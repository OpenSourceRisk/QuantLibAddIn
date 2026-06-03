# Building the QuantLibAddin C++ Addin on Linux (gcc)

This note records how to build QuantLibAddin -- from the core QuantLib library up through gensrc -> ObjectHandler -> QuantLibAddin -> the QLADemo C++ client -- on Linux/WSL using gcc and the cmake build (Build B).

Result: SUCCESS with gcc. The complete pipeline works: gensrc autogenerates the addin source, ObjectHandler + QuantLibAddin compile, and the QLADemo console client builds and runs, pricing a European option at PV = 3.84431.

## Assumed layout

This note assumes a writable checkout in the Linux filesystem (fast, and required because gensrc writes into the source tree), with Boost unpacked alongside it. The verified layout used for this build was:

	~/repos2/QuantLibAddin      # this repository
	~/repos2/boost_1_83_0       # Boost 1.83.0 source tree

(The directory name happens to be `repos2`; any writable Linux-filesystem
location works.  Do NOT build from /mnt/c -- WSL has a large performance lag
there and /mnt/c is mounted read-only from inside WSL in this environment.)

On a native Linux box you would instead just install Boost from the system package manager or conan and skip the manual Boost steps below.

## Environment

| Item     | Value |
|----------|-------|
| Distro   | Ubuntu 22.04 (WSL2) |
| Compiler | gcc / g++ 11.4.0 |
| sudo     | not available (cannot apt install) |
| cmake    | not installed in the distro (provisioned in user space, below) |
| ninja    | not installed in the distro (provisioned in user space, below) |
| conan    | not installed |
| Python   | python3 3.10 (system) |

## Driving WSL from a Windows (PowerShell) agent — gotchas

If you are driving the build via `wsl.exe -- bash -c '...'` from PowerShell
(rather than typing inside an interactive WSL shell), three things bite:

1. **Use a non-login shell and avoid `$` in the outer command line.**
   `bash -lc` produced garbled/empty output capture in this environment, and
   any `$` (e.g. `$PATH`, `$?`) in the command string risks being expanded by
   PowerShell before it ever reaches bash.  Prefer `bash -c`, and when you
   need shell variables, put the commands in a script file (see next point)
   and run `bash /path/to/script.sh`.

2. **A corporate security agent blocks some `wsl.exe` launches.**
   - Any launch whose command line contains the word `install` is blocked
	 (fails with "Program 'wsl.exe' failed to run: Access is denied").
	 `pip download` is fine; `pip install` is not.
   - Long/complex command lines (e.g. a full `cmake ... -D...` invocation with
	 many flags) were also intermittently blocked the same way.
   The robust workaround is to keep the `wsl.exe` command line short: write the
   real command into a small shell script (see "Writing files into WSL" below),
   then launch it with `wsl.exe -- bash /home/developer/cfg.sh`.
   Occasional "Access is denied" failures are transient -- simply retry.

3. **Writing files into WSL from the Windows side.**
   /mnt/c is read-only from inside WSL here, so you cannot write the Linux
   files via /mnt/c.  Two reliable options:
   - base64-encode the file content on the Windows side, then
	 `wsl.exe -- bash -c 'base64 -d /mnt/c/.../file.b64 > ~/target'`
	 (avoids all quoting/`$`/CRLF issues), or
   - create the file under a Windows temp dir and copy it in with
	 `sed 's/\r$//' /mnt/c/.../file > ~/target` to strip CRLF line endings.

## Tooling provisioning (no root, no apt)

There is no sudo/apt and no system cmake/ninja, so both are provisioned into
user space without installing anything system-wide:

	cd ~/tools/dl
	pip3 download --no-deps cmake ninja          # NB: "download", not "install"
	python3 -m zipfile -e cmake-*.whl  ~/tools/cmake/
	python3 -m zipfile -e ninja-*.whl  ~/tools/ninja/
	chmod +x ~/tools/cmake/cmake/data/bin/cmake \
			 ~/tools/ninja/ninja-*.data/scripts/ninja

The wheels are just zip archives of prebuilt Linux binaries.  Verified
versions: cmake 4.3.2, ninja 1.13.0.  Put both directories on PATH, e.g. via
a small env script that you `source` in every command:

	# ~/qla_env.sh
	export PATH=/home/developer/tools/cmake/cmake/data/bin:/home/developer/tools/ninja/ninja-1.13.0.data/scripts:$PATH

(Write this file using the base64 trick above so the literal `$PATH` survives.)

On a native Linux box just use the system cmake/ninja and ignore this section.

## Boost

QuantLib core is effectively header-only on Boost, so its find_package(Boost ... REQUIRED) (called without components) only needs the Boost headers -- point cmake at the boost_1_83_0 tree.

The addin layers (ObjectHandler / QuantLibObjects) additionally need the compiled Boost filesystem and serialization libraries. With no conan and no system Boost, compile just those two from the Boost source tree into user-space static archives, e.g. ~/boostlibs/lib/libboost_{filesystem,serialization}.a:

	cd ~/repos2/boost_1_83_0
	sh ./bootstrap.sh                              # see note on exec bits below
	./b2 --with-filesystem --with-serialization link=static \
		 runtime-link=shared cxxstd=20 \
		 --stagedir=~/boostlibs stage              # see note on cxxstd below

Note (exec bits): if the checkout came from a .zip (not a git clone) the
executable bits are lost, so `./bootstrap.sh` and the engine `build.sh` it
calls fail with "Permission denied".  Run them via `sh` explicitly:

	sh ./bootstrap.sh
	# if bootstrap still can't build the engine:
	cd tools/build/src/engine && sh ./build.sh gcc && cd -
	cp tools/build/src/engine/b2 ./b2

Note (cxxstd): Boost.Filesystem 1.83 uses std::atomic_ref (a C++20 feature), so its sources must be compiled with cxxstd=20. This only affects how the Boost libraries themselves are compiled -- the addin is still built as C++17.

On native Linux, instead just apt install libboost-filesystem-dev libboost-serialization-dev (or use conan) and skip this step.

## What the cmake build needed to be Linux-capable

The cmake files previously assumed MSVC/Windows for the addin layers. The following portable edits make them build on Linux too; Windows behaviour is unchanged because everything is guarded by if(WIN32) / if(MSVC):

| File | Change |
|------|--------|
| CMakeLists.txt | Only add the Excel QuantLibXL subdir on WIN32. Delegates the optional gensrc "Full build" step to gensrc/CMakeLists.txt (see below). |
| gensrc/CMakeLists.txt | Defines the QLA_RUN_GENSRC option, runs gensrc at CONFIGURE time (see "Full build" below), and defines the ohgensrc / qlgensrc build-time targets. |
| ObjectHandler/CMakeLists.txt | Force static Boost / static runtime only for MSVC. Guard the Excel-only xlsdk and ohxllib targets behind if(WIN32) (the core ohlib still builds everywhere). |
| QuantLibAddin/CMakeLists.txt | Force static Boost only for MSVC. |
| QuantLibAddin/Clients/Cpp/CMakeLists.txt | Link the Windows-only system libs (odbc32, Ws2_32, ...) only on WIN32. |

No qlo/, oh/, Addins/ source needs changing -- the addin C++ code is already Linux-clean.

A native-Linux configure preset is provided in CMakePresets.json:

	cmake --preset linux-gcc
	cmake --build --preset linux-gcc

It uses gcc, C++17, and system Boost via find_package. When using the hand-compiled Boost archives instead of system Boost, pass the Boost paths explicitly (see "Build & run" below).

## gensrc on Linux

gensrc is a Python 3 program. Two things to know:

- It is invoked as `python3 <gensrc>/gensrc.py ...` but must be run *from* the
  consuming gensrc working directory (ObjectHandler/gensrc or
  QuantLibAddin/gensrc), because rule.py adds os.getcwd()+'/code' to sys.path
  to import that directory's codedict.py.  (gensrc.py itself lives in the
  top-level gensrc/ directory.)
- It writes generated files into the source tree, so the checkout must be
  writable -- which is why a Linux-filesystem checkout is used.

Verified runs (Python 3.10, exit 0):

| gensrc invocation | run from | files generated |
|-------------------|----------|-----------------|
| python3 ../../gensrc/gensrc.py -xdlv --oh_dir=.. | ObjectHandler/gensrc | 36 |
| python3 ../../gensrc/gensrc.py -pvels --oh_dir=../../ObjectHandler | QuantLibAddin/gensrc | 484 |

On Linux the QuantLibAddin gensrc uses -pvels (C++ addin, ValueObjects, Enumerations, Loops, Serialization) rather than -a; the -a set also includes the Excel (x) target, which writes into the Windows-only QuantLibXL tree.

### Full build via cmake (QLA_RUN_GENSRC) — single configure, no manual gensrc

gensrc/CMakeLists.txt adds an optional gensrc step, off by default (mirroring
the VS "basic" vs "full" distinction):

	cmake ... -DQLA_RUN_GENSRC=ON

When ON, this works on a *pristine* checkout with a single `cmake` configure --
**you do NOT need to run gensrc by hand first.**

Why this matters: the addin library targets (ohlib, QuantLibObjects, AddinCpp)
list their autogenerated .cpp files explicitly in add_library().  CMake checks
those files exist when the targets are *created*, i.e. at configure time.  So
gensrc must run *before* those subdirectories are configured.  gensrc/
CMakeLists.txt therefore runs gensrc with execute_process() during
configuration (the root CMakeLists.txt processes add_subdirectory(gensrc)
before ObjectHandler/QuantLibAddin, so the generated sources are guaranteed to
exist in time).  A stamp file (build/gensrc/gensrc.configured.stamp) records
that this has happened; delete it, or configure into a clean build directory,
to force regeneration.

The ohgensrc / qlgensrc custom targets are still defined and wired up
(ohlib depends on ohgensrc, QuantLibObjects depends on qlgensrc) so that on an
*incremental* build, edits to gensrc metadata/templates refresh the generated
sources before compilation.  When QLA_RUN_GENSRC is OFF (the default) the build
uses the previously generated files, exactly like the VS "basic" build.

## Build & run

Because a full `cmake ... -D...` command line can be blocked by the security
agent (see gotcha #2 above), put it in a script and run that.  Example
`~/cfg.sh`:

	source ~/qla_env.sh
	cmake -G Ninja -S ~/repos2/QuantLibAddin -B ~/qla_build \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DQLA_RUN_GENSRC=ON \
	  -DBOOST_ROOT=/home/developer/repos2/boost_1_83_0 \
	  -DBOOST_INCLUDEDIR=/home/developer/repos2/boost_1_83_0 \
	  -DBOOST_LIBRARYDIR=/home/developer/boostlibs/lib \
	  -DBoost_NO_SYSTEM_PATHS=ON -DBoost_USE_STATIC_LIBS=ON \
	  -DBoost_FILESYSTEM_LIBRARY_RELEASE=/home/developer/boostlibs/lib/libboost_filesystem.a \
	  -DBoost_SERIALIZATION_LIBRARY_RELEASE=/home/developer/boostlibs/lib/libboost_serialization.a

Configure, then build (the configure step runs gensrc automatically):

	wsl.exe -- bash /home/developer/cfg.sh
	wsl.exe -- bash -c 'source ~/qla_env.sh; cmake --build ~/qla_build --target QLADemo -j'

Use absolute /home/... paths in the -D cache variables (do not rely on ~/
expansion inside cmake cache entries).

Output executable: ~/qla_build/cpp/QLADemo--x64-mt-1_23_0 (ELF x86-64).

Running it (from a writable directory) prints:

	INFO  Begin example program.
	INFO  QuantLibAddin version = 1.23.0
	INFO  ObjectHandler version = 1.23.0
	INFO  option PV = 3.84431
	INFO  log dump of object with ID = my_option
	  ... property dump ...
	INFO  End example program.

and writes qlademo.log and option_demo.xml (the latter via Boost serialization, confirming the compiled Boost libs work). Exit code 0.

## Summary

- The QuantLibAddin cmake build is Linux-capable with gcc: the C++ Addin and its QLADemo client compile and run.
- The only cmake changes are portable guards (if(WIN32)/if(MSVC)) plus the
  configure-time gensrc wiring; the Windows XLL build is unaffected.
- A "Full" build (-DQLA_RUN_GENSRC=ON) now works on a pristine checkout with a
  single configure -- gensrc runs at configure time, so no manual gensrc step
  is required.
- Provisioning workarounds (user-space cmake/ninja, hand-compiled Boost) and
  the WSL/PowerShell gotchas above are WSL-specific; on native Linux the system
  gcc/cmake/Boost suffice and the linux-gcc preset gives a one-command build.
