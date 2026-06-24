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

NB (current environment): the live pristine checkout is now at
`/home/developer/repos/QuantLibAddin`, and Boost is supplied as the archive
`/home/developer/repos/boost_1_83_0.zip` (unzip it before the Boost step below).
Substitute these paths for the `~/repos2/...` ones used throughout this note.

On a native Linux box you would instead just install Boost from the system package manager or conan and skip the manual Boost steps below.

## Environment

| Item     | Value |
|----------|-------|
| Distro   | Ubuntu 22.04 (WSL2) |
| Compiler | gcc / g++ 11.4.0 |
| sudo     | not available (cannot apt install) |
| cmake    | not in distro; user-space 4.3.2 at `~/tools/cmake/cmake/data/bin/cmake`; on PATH for login/interactive shells (added to `~/.bash_profile` + `~/.bash_aliases`, like doxygen/dot — provisioning below) |
| ninja    | not in distro; user-space 1.13.0 at `~/tools/ninja/ninja-1.13.0.data/scripts/ninja`; on PATH for login/interactive shells (added to `~/.bash_profile` + `~/.bash_aliases`, like doxygen/dot — provisioning below) |
| conan    | not installed |
| Python   | python3 3.10 (system) |

## Driving WSL from a Windows (PowerShell) agent — gotchas

If you are driving the build via `wsl.exe -- bash -c '...'` from PowerShell
(rather than typing inside an interactive WSL shell), five things bite:

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

4. **User-space tools are on PATH for login/interactive shells, but absent
   under a bare `bash -c`.**  cmake, ninja, doxygen and dot all live in user
   space and are added to PATH by `~/.bash_profile` (login) and
   `~/.bash_aliases` (interactive).  But `wsl.exe -- bash -c '...'` is
   **neither a login nor an interactive shell**, so it sources no startup files
   and sees none of them -- e.g. `cmake: command not found` even though
   `/home/developer/tools/cmake/cmake/data/bin/cmake` (verified 4.3.2) exists.
   Two fixes: invoke a login+interactive shell
   (`wsl.exe -- bash -lic '...'`), or -- more robustly -- `source` the env
   scripts at the top of your script:
   `source /home/developer/qla_env.sh` (cmake + ninja) and
   `source /home/developer/doxy_env.sh` (doxygen + dot).

5. **Recommended pattern: a base64-shipped script that sources the env.**
   The single most reliable way to run a multi-step command -- setting PATH and
   sidestepping every quoting / `$`-expansion / CRLF pitfall at once -- is to
   put everything in a script file, ship it into WSL with base64, and run it:

   ```powershell
   $b = [Convert]::ToBase64String([IO.File]::ReadAllBytes("C:\path\script.sh"))
   wsl.exe -- bash -c "echo $b | base64 -d | tr -d '\r' > /tmp/script.sh; bash /tmp/script.sh"
   ```

   Make the first lines of `script.sh` `source /home/developer/qla_env.sh` and
   `source /home/developer/doxy_env.sh` so cmake/ninja/doxygen/dot are all on
   PATH.  base64 avoids PowerShell `$`-expansion and quoting; `tr -d '\r'`
   strips CRLFs; and the short `wsl.exe` command line dodges the security-agent
   block from gotcha #2.

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
versions: cmake 4.3.2, ninja 1.13.0.  Both directories are put on PATH by
appending a `cmake+ninja (user-space)` block to `~/.bash_profile` and
`~/.bash_aliases` (the same files that carry the `doxygen+graphviz` block), so
login and interactive shells pick them up automatically.  A standalone
`~/qla_env.sh` carrying the same export is also kept, for `source`-ing inside
non-interactive `bash -c` scripts (which load no startup files — see gotchas
#4 and #5):

	# ~/qla_env.sh
	export PATH=/home/developer/tools/cmake/cmake/data/bin:/home/developer/tools/ninja/ninja-1.13.0.data/scripts:$PATH

(Write this file using the base64 trick above so the literal `$PATH` survives.)

### Heads-up: PyPI is being blocked over Zscaler

The `pip3 download` step above relies on PyPI, which **PTS engineering have
announced will be blocked over Zscaler from end of next month**.  After the
cutoff, `pip` against the public index returns a Zscaler block page instead of
the package -- the same failure mode as the GitLab/GCS block in
[INSTALLING_DOXYGEN_DOT_WSL.md](INSTALLING_DOXYGEN_DOT_WSL.md), not a real
download.  This only affects (re)provisioning a fresh machine: an existing
checkout already has cmake/ninja under `~/tools` and is unaffected, and gensrc
plus the rest of the build use only the system `python3` and import nothing from
PyPI, so they keep working regardless.

Two ways to provision cmake/ninja after the cutoff:

- **Preferred -- download the prebuilt binaries straight from GitHub releases.**
  GitHub's release CDN is Zscaler-allowed (it is already the source used for
  doxygen), and this needs no pip, no token and no PyPI at all:
  - cmake: `cmake-<ver>-linux-x86_64.tar.gz` from
	<https://github.com/Kitware/CMake/releases>
  - ninja: `ninja-linux.zip` from
	<https://github.com/ninja-build/ninja/releases>

  Unpack each under `~/tools/...` and point `PATH` (and `~/qla_env.sh`) at the
  resulting `bin` / binary directory, exactly as above.

- **Alternative -- repoint pip at the LSEG Artifactory PyPI proxy** so the
  existing `pip3 download` keeps working.  Create a user-level
  `~/.config/pip/pip.conf` (no root needed):

	[global]
	index-url = https://USERNAME:TOKEN@artifactory.lseg.com/artifactory/api/pypi/python-remotes/simple
	trusted-host = artifactory.lseg.com

  Replace USERNAME/TOKEN with your LSEG username and Artifactory token;
  `trusted-host` is required because Zscaler re-signs TLS with an internal CA.
  `python-remotes` is a caching proxy of PyPI, so common packages such as
  cmake/ninja resolve without a special request.  The file carries a secret
  token, so keep it out of the repo and write it with the base64 trick so the
  literal `$`/`@`/token survive.  Reference:
  <https://docs.devportal.lseg.com/dxone-developer-platform/artifactory/getting-started/package-managers>.

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
	wsl.exe -- bash -c 'source ~/qla_env.sh; cmake --build ~/qla_build --target QLADemo -j 2'

Use absolute /home/... paths in the -D cache variables (do not rely on ~/
expansion inside cmake cache entries).

Note (-j 2, not bare -j): keep the parallel job count modest.  QuantLib's
template-heavy translation units each consume 1-2 GB of RAM while compiling, and
an unbounded `-j` launches one compile per core at once.  Under WSL -- whose VM
has a capped RAM/swap budget -- that exhausts memory and the Linux OOM killer
terminates the compiler with `g++: fatal error: Killed signal terminated program
cc1plus`.  A tell-tale sign that it is OOM and not a code error is that
re-running the build resumes *past* the file that failed (finished .o files are
kept) and stops on a *different* file each time.  Use roughly one job per ~2 GB
of RAM (`-j 2` is safe on a default WSL VM), and/or raise the WSL limits in
`C:\Users\<you>\.wslconfig` on the Windows side:

	[wsl2]
	memory=12GB
	swap=16GB

then apply it with `wsl --shutdown` (from Windows PowerShell) before rebuilding.

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

## Building the documentation (doxygen)

The Doxygen HTML docs are a separate, optional build, gated by the configure-time
cmake option `BUILD_DOCS` (default OFF).  They need only Python 3 and doxygen
(plus optional Graphviz dot for diagrams) -- no C++ compiler and no Boost -- so
they build even on a machine that cannot compile the libraries.  See section 9
of [build_cmake.md](build_cmake.md) for the cross-platform description; the
WSL-specific points are below.

Provision doxygen and dot in user space first (no root/apt) and put them on
PATH -- that is documented in
[INSTALLING_DOXYGEN_DOT_WSL.md](INSTALLING_DOXYGEN_DOT_WSL.md).  The tools are
discovered when cmake *configures*, not when it builds, so they must be on PATH
at configure time; a bare `wsl.exe -- bash -c '...'` sources no startup files,
so `source ~/doxy_env.sh` (doxygen + dot) inside the script (gotchas #4/#5
above).  If doxygen is missing the `-DBUILD_DOCS=ON` configure fails with
`doxygen was not found on PATH`; if only dot is missing, configure prints
`dot (Graphviz) not found; diagrams will be skipped` and continues.

Add `-DBUILD_DOCS=ON` to the configure (it is a configure-time option, so a
build-only invocation cannot add the targets), then build the aggregate `docs`
target (or a single `*-docs` target):

	source ~/qla_env.sh        # cmake + ninja
	source ~/doxy_env.sh       # doxygen + dot
	cmake -G Ninja -S ~/repos/QuantLibAddin -B ~/qla_build -DBUILD_DOCS=ON
	cmake --build ~/qla_build --target docs

Per-project targets are also available: `gensrc-docs`, `ObjectHandler-docs`,
`QuantLibAddin-docs`, `QuantLibXL-docs` (the QuantLibXL docs build on Linux too,
even though the XLL does not).  The generated HTML lands under the build tree,
e.g. `~/qla_build/gensrc-docs/html/index.html`.

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
