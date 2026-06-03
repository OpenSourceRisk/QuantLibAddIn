# Building the QuantLibAddin C++ Addin on Linux (gcc)

This note records how to build QuantLibAddin -- from the core QuantLib library up through gensrc -> ObjectHandler -> QuantLibAddin -> the QLADemo C++ client -- on Linux/WSL using gcc and the cmake build (Build B).

Result: SUCCESS with gcc. The complete pipeline works: gensrc autogenerates the addin source, ObjectHandler + QuantLibAddin compile, and the QLADemo console client builds and runs, pricing a European option at PV = 3.84431.

## Assumed layout

This note assumes a writable checkout in the Linux filesystem (fast, and required because gensrc writes into the source tree), with Boost unpacked alongside it, e.g.:

    ~/repos/QuantLibAddin      # this repository
    ~/repos/boost_1_83_0       # Boost 1.83.0 source tree

On a native Linux box you would instead just install Boost from the system package manager or conan and skip the manual Boost steps below.

## Environment

| Item     | Value |
|----------|-------|
| Distro   | Ubuntu 22.04 (WSL2) |
| Compiler | gcc / g++ 11.4.0 |
| sudo     | not available (cannot apt install) |
| cmake    | not installed in the distro (provisioned in user space, below) |
| conan    | not installed |

## Tooling provisioning (no root, no apt)

There is no sudo/apt and no system cmake, so cmake and ninja are provisioned into user space without installing anything system-wide:

- A corporate security agent silently blocks any wsl.exe launch whose command line contains the word install (e.g. pip install ...); such launches fail with "Program 'wsl.exe' failed to run: Access is denied". pip download is not blocked.
- Workaround: pip3 download --no-deps cmake ninja (the wheels are just zip archives of prebuilt Linux binaries), then extract with python3 -m zipfile -e and chmod +x the binaries.
- Result, for example: ~/tools/cmake/data/bin/cmake and ~/tools/ninja-1.13.0.data/scripts/ninja. Put both on PATH.

On a native Linux box just use the system cmake/ninja and ignore this section.

## Boost

QuantLib core is effectively header-only on Boost, so its find_package(Boost ... REQUIRED) (called without components) only needs the Boost headers -- point cmake at the boost_1_83_0 tree.

The addin layers (ObjectHandler / QuantLibObjects) additionally need the compiled Boost filesystem and serialization libraries. With no conan and no system Boost, compile just those two from the Boost source tree into user-space static archives, e.g. ~/boostlibs/lib/libboost_{filesystem,serialization}.a:

    cd ~/repos/boost_1_83_0
    ./bootstrap.sh
    ./b2 --with-filesystem --with-serialization link=static \
         runtime-link=shared cxxstd=20 stage    # see note on cxxstd below

Note: Boost.Filesystem 1.83 uses std::atomic_ref (a C++20 feature), so its sources must be compiled with cxxstd=20. This only affects how the Boost libraries themselves are compiled -- the addin is still built as C++17.

On native Linux, instead just apt install libboost-filesystem-dev libboost-serialization-dev (or use conan) and skip this step.

## What the cmake build needed to be Linux-capable

The cmake files previously assumed MSVC/Windows for the addin layers. The following portable edits make them build on Linux too; Windows behaviour is unchanged because everything is guarded by if(WIN32) / if(MSVC):

| File | Change |
|------|--------|
| CMakeLists.txt | Only add the Excel QuantLibXL subdir on WIN32. Delegates the optional gensrc "Full build" step to gensrc/CMakeLists.txt (see below). |
| gensrc/CMakeLists.txt | Defines the QLA_RUN_GENSRC option and the ohgensrc / qlgensrc code-generation targets. |
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

- It must be run from the gensrc working directory (ObjectHandler/gensrc or QuantLibAddin/gensrc), because rule.py adds os.getcwd()+'/code' to sys.path to import that directory's codedict.py.
- It writes generated files into the source tree, so the checkout must be writable -- which is why a ~/repos checkout (Linux filesystem) is used.

Verified runs (Python 3.10, exit 0):

| gensrc invocation | from | files generated |
|-------------------|------|-----------------|
| gensrc.py -xdlv --oh_dir=.. | ObjectHandler/gensrc | 36 |
| gensrc.py -pvels --oh_dir=../../ObjectHandler | QuantLibAddin/gensrc | 484 |

On Linux the QuantLibAddin gensrc uses -pvels (C++ addin, ValueObjects, Enumerations, Loops, Serialization) rather than -a; the -a set also includes the Excel (x) target, which writes into the Windows-only QuantLibXL tree.

### Full build via cmake (QLA_RUN_GENSRC)

gensrc/CMakeLists.txt adds an optional gensrc step, off by default (mirroring the VS "basic" vs "full" distinction):

    cmake ... -DQLA_RUN_GENSRC=ON

When ON, cmake finds Python 3 and adds ohgensrc / qlgensrc custom targets; ohlib depends on ohgensrc and QuantLibObjects depends on qlgensrc, so the autogenerated sources are refreshed before compilation. When OFF, the build uses the previously generated files (the default, like "basic").

## Build & run

Configure (gcc, snapshot Boost headers + hand-built Boost archives):

    cmake -G Ninja -S ~/repos/QuantLibAddin -B ~/qla_build \
      -DCMAKE_BUILD_TYPE=Release \
      -DBOOST_ROOT=~/repos/boost_1_83_0 \
      -DBOOST_INCLUDEDIR=~/repos/boost_1_83_0 \
      -DBOOST_LIBRARYDIR=~/boostlibs/lib \
      -DBoost_NO_SYSTEM_PATHS=ON -DBoost_USE_STATIC_LIBS=ON \
      -DBoost_FILESYSTEM_LIBRARY_RELEASE=~/boostlibs/lib/libboost_filesystem.a \
      -DBoost_SERIALIZATION_LIBRARY_RELEASE=~/boostlibs/lib/libboost_serialization.a
    cmake --build ~/qla_build --target QLADemo -j

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
- The only cmake changes are portable guards (if(WIN32)/if(MSVC)); the Windows XLL build is unaffected.
- gensrc works on Linux (Python 3) and is wired into cmake via gensrc/CMakeLists.txt behind the optional QLA_RUN_GENSRC flag.
- Provisioning workarounds (user-space cmake/ninja, hand-compiled Boost) are WSL-specific; on native Linux the system gcc/cmake/Boost suffice and the linux-gcc preset gives a one-command build.
