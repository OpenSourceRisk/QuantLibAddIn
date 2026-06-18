# Build QuantLibAddin from a git clone using cmake

This document explains how to build QuantLibAddin - the QuantLib wrapper layer
(`QuantLibObjects`), its C++ add-in (`AddinCpp`) and the C++ demo client
(`QLADemo`) - from a **git clone**, using the **cmake** build. It is the cmake
counterpart of [build_vs.md](build_vs.md) (the hand-maintained Visual Studio
solution build); both use the same source files and produce the same libraries.

QuantLibAddin is normally built as a prerequisite of QuantLibXL. If your goal is
the QuantLibXL Excel add-in, follow [../../QuantLibXL/md/build_cmake.md](../../QuantLibXL/md/build_cmake.md) instead - it
builds QuantLibAddin for you as part of the dependency chain. Use this document
when you want to build or test the QuantLibAddin layer (or its C++ client) on
its own.

For other options:

- To download a compiled QuantLibXL XLL, see
  <https://www.quantlib.org/quantlibxl/installation.html>.
- To compile various flavors of QuantLibAddin (including the QuantLibXL XLL) from
  an official release of source code (zip files / tarballs), see
  <https://www.quantlib.org/quantlibaddin/tutorials.html>.

For the full reference description of the cmake build (file layout, design
notes, standalone subproject builds and the documentation build) see
[../../README_cmake_build.md](../../README_cmake_build.md) in the repository root. This HOWTO covers the common
case: building the QuantLibAddin libraries and demo client from a clone.

---

## 1 Choose a build - Basic or Full

Both builds produce the same libraries; they differ only in whether the
auto-generated source code is regenerated. The choice is a single cmake cache
variable, `RUN_GENSRC`, set at configure time.

| Build | Configure with | Description |
|---|---|---|
| Basic | (default - `RUN_GENSRC` is `OFF`) | Compiles the auto-generated source files that are already present in the clone. Does **not** require Python. Use this for normal compilation. |
| Full  | `-DRUN_GENSRC=ON` | Runs gensrc first to regenerate all auto-generated source files, then compiles. Requires Python 3. Use this only if you have changed the gensrc metadata (the XML files under `ObjectHandler\gensrc\metadata` or `QuantLibAddin\gensrc\metadata`). |

Unless you are editing the add-in's metadata, use the **Basic** build (just
omit `RUN_GENSRC`).

---

## 2 Prerequisites

### 2.1 CMake and a C++ compiler

- **CMake 3.15 or later.** CMake ships with the **"C++ CMake tools for
  Windows"** component of the Visual Studio **"Desktop development with C++"**
  workload, so if you installed that workload you already have it. Check with
  `cmake --version`.
- **Visual Studio 2026 or 2022** with the **"Desktop development with C++"**
  workload. The cmake build is **x64 only**.

The platform toolset is selected automatically by the generator named in the
preset you choose (section 4); no manual toolset configuration is required:

| Visual Studio | Generator (from the preset) | Platform toolset |
|---|---|---|
| VS 2022 (v17) | `Visual Studio 17 2022` | v143 |
| VS 2026 (v18) | `Visual Studio 18 2026` | v145 |

### 2.2 Boost

QuantLib and QuantLibAddin both depend on Boost. You need the **compiled** Boost
libraries, not just the headers (the build links the Boost `filesystem` and
`serialization` components). Building Boost is outside the scope of this
document; these instructions assume you already have a Boost build available.

The build requires **Boost 1.58 or later**. It was tested using **Boost 1.83**.

For purposes of this HOWTO, it is assumed that you have installed Boost to:

```
C:\repos\boost_1_83_0
```

with headers under `C:\repos\boost_1_83_0` (i.e. the folder that contains the
`boost\` sub-directory) and the compiled libraries under
`C:\repos\boost_1_83_0\stage\lib`. Modify that path as necessary for your own
environment.

Unlike the Visual Studio build, the cmake build has a **single** Boost
configuration point: you supply the include and library directories once, in
`CMakeUserPresets.json`, and cmake applies them to every project in the build
(QuantLib included). This is described in section 4.

### 2.3 Python 3 (Full build only)

The **Full** build (`-DRUN_GENSRC=ON`) runs gensrc, which is a Python 3 script.
Install Python 3 and make sure `python` is on the `PATH` (i.e. `python
--version` works from a command prompt). See <https://www.python.org/>. When
`RUN_GENSRC` is on, cmake locates the interpreter itself via `find_package`.

The **Basic** build does not use Python or gensrc, so you can skip this section
for a Basic build.

---

## 3 Acquire the source code

The build refers to the prerequisite projects using **relative paths**, so the
directory layout matters. After cloning you must end up with this layout
(`QuantLibAddin` is the repository root - you may name the outer folder anything
you like; it is also where `CMakeLists.txt` and `CMakePresets.json` live):

```
QuantLibAddin\
  gensrc\         (required for the Full build only)
  ObjectHandler\
  QuantLib\       (cloned separately - see below)
  QuantLibAddin\
```

### 3.1 Clone the main repository

`gensrc`, `ObjectHandler` and `QuantLibAddin` are all contained in a single
repository. Clone it:

```
git clone https://gitlab.dx1.lseg.com/app/app-51172/qs/QuantLibAddin.git
```

This creates the `QuantLibAddin` folder containing the sub-projects above.

### 3.2 Clone QuantLib into the working tree

QuantLibAddin is the add-in wrapper for **QuantLib**, so the cmake build
compiles and links QuantLib: the root `CMakeLists.txt` adds it with
`add_subdirectory(QuantLib)` and `QuantLibObjects` links the resulting library.
QuantLib is maintained as a **separate** repository and is deliberately excluded
from the main repository (it is listed in `.gitignore`), so you must clone it
yourself into a sub-folder named exactly `QuantLib` inside the working tree you
just cloned:

```
cd QuantLibAddin
git clone https://gitlab.dx1.lseg.com/app/app-51172/qs/quantlib QuantLib
```

The folder name `QuantLib` **is case sensitive** and must be spelled exactly as
shown - `QuantLib`, not `quantlib` or `QUANTLIB`, and with no version suffix -
because `CMakeLists.txt` adds the sub-directory by that name. Note that the
`git clone` command above ends with an explicit `QuantLib` argument for this
reason; without it git would create a folder named `quantlib` from the URL.

After this step the layout in section 3 should be in place.

---

## 4 Configure Boost (required)

Boost paths are **not** baked into the shared `CMakePresets.json`. Instead each
user supplies them in a `CMakeUserPresets.json` file in the repository root
(next to `CMakePresets.json`). That file is listed in `.gitignore` and is never
committed, so your local paths stay out of the repository.

`CMakePresets.json` defines hidden **base** presets (compiler, architecture and
runtime selection); your `CMakeUserPresets.json` defines the presets you
actually use, each **inheriting** a base preset and adding your Boost paths.
Create it by copying the template below and adjusting the two Boost paths:

```json
{
    "version": 6,
    "configurePresets": [
        {
            "name": "windows-vs2026-x64-static",
            "displayName": "VS 2026 x64 - static CRT",
            "inherits": "windows-vs2026-x64-static-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/repos/boost_1_83_0",
                "BOOST_LIBRARYDIR": "C:/repos/boost_1_83_0/stage/lib"
            }
        },
        {
            "name": "windows-vs2026-x64-dynamic",
            "displayName": "VS 2026 x64 - dynamic CRT",
            "inherits": "windows-vs2026-x64-dynamic-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/repos/boost_1_83_0",
                "BOOST_LIBRARYDIR": "C:/repos/boost_1_83_0/stage/lib"
            }
        },
        {
            "name": "windows-vs2022-x64-static",
            "displayName": "VS 2022 x64 - static CRT",
            "inherits": "windows-vs2022-x64-static-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/repos/boost_1_83_0",
                "BOOST_LIBRARYDIR": "C:/repos/boost_1_83_0/stage/lib"
            }
        },
        {
            "name": "windows-vs2022-x64-dynamic",
            "displayName": "VS 2022 x64 - dynamic CRT",
            "inherits": "windows-vs2022-x64-dynamic-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/repos/boost_1_83_0",
                "BOOST_LIBRARYDIR": "C:/repos/boost_1_83_0/stage/lib"
            }
        }
    ],
    "buildPresets": [
        { "name": "windows-vs2026-x64-static-release",  "configurePreset": "windows-vs2026-x64-static",  "configuration": "Release" },
        { "name": "windows-vs2026-x64-static-debug",    "configurePreset": "windows-vs2026-x64-static",  "configuration": "Debug"   },
        { "name": "windows-vs2026-x64-dynamic-release", "configurePreset": "windows-vs2026-x64-dynamic", "configuration": "Release" },
        { "name": "windows-vs2026-x64-dynamic-debug",   "configurePreset": "windows-vs2026-x64-dynamic", "configuration": "Debug"   },
        { "name": "windows-vs2022-x64-static-release",  "configurePreset": "windows-vs2022-x64-static",  "configuration": "Release" },
        { "name": "windows-vs2022-x64-static-debug",    "configurePreset": "windows-vs2022-x64-static",  "configuration": "Debug"   },
        { "name": "windows-vs2022-x64-dynamic-release", "configurePreset": "windows-vs2022-x64-dynamic", "configuration": "Release" },
        { "name": "windows-vs2022-x64-dynamic-debug",   "configurePreset": "windows-vs2022-x64-dynamic", "configuration": "Debug"   }
    ]
}
```

Notes:

- `BOOST_INCLUDEDIR` is the directory that **contains** the `boost\` header
  sub-folder (so that `#include <boost/config.hpp>` resolves).
- `BOOST_LIBRARYDIR` is the directory that contains the compiled `.lib` files.
  Boost auto-linking is disabled in this build, so cmake selects the correct
  libraries (including `filesystem` and `serialization`) from that directory;
  you do not list individual libraries.
- Paths in JSON use **forward slashes** (or escaped `\\`). The `static` presets
  request the static-runtime Boost libraries and the `dynamic` presets the
  dynamic-runtime ones. If your static- and dynamic-runtime Boost libraries
  live in separate directories, point each preset's `BOOST_LIBRARYDIR` at the
  matching folder; if a single `b2` build placed all variants in one `stage\lib`
  directory (the assumption above), the same path works for both.
- You only need the presets you intend to use - delete the others from your
  `CMakeUserPresets.json` if you build with just one compiler.

If Boost is installed somewhere cmake can find automatically (for example via a
`BOOST_ROOT` environment variable or vcpkg), `CMakeUserPresets.json` can be
omitted entirely and the shared presets will work as-is.

---

## 5 Configure

Open a **Developer PowerShell / Command Prompt** (or any shell where `cmake` is
on the `PATH`), change to the repository root - the `QuantLibAddin` folder that
contains `CMakePresets.json` - and run cmake with the preset you want. The
preset's `binaryDir` puts the build under `build\<preset>\` automatically:

```powershell
# VS 2026, static CRT  (recommended for distribution)
cmake --preset windows-vs2026-x64-static

# VS 2026, dynamic CRT
cmake --preset windows-vs2026-x64-dynamic

# VS 2022, static CRT
cmake --preset windows-vs2022-x64-static

# VS 2022, dynamic CRT
cmake --preset windows-vs2022-x64-dynamic
```

For a **Full** build (regenerate the auto-generated sources first - requires
Python 3), add `-DRUN_GENSRC=ON` to the configure command, e.g.:

```powershell
cmake --preset windows-vs2026-x64-static -DRUN_GENSRC=ON
```

gensrc then runs during configuration and regenerates the source code before any
target is built. For a Basic build, simply omit `-DRUN_GENSRC=ON`.

---

## 6 Build

The QuantLibAddin subproject produces three targets:

| Target | Kind | Description |
|---|---|---|
| `QuantLibObjects` | static library | the QuantLib/ObjectHandler wrapper layer |
| `AddinCpp` | static library | the C++ add-in layer (links `QuantLibObjects`) |
| `QLADemo` | executable | the C++ demo client (links `AddinCpp`) |

Building `QLADemo` pulls in everything it depends on - `AddinCpp`,
`QuantLibObjects`, QuantLib (`ql_library`) and the ObjectHandler static
libraries - in dependency order, so it is the simplest single target to build.
Choose the configuration with `--config`; because the Visual Studio generator is
multi-config, the **same** build directory produces either Release or Debug:

```powershell
# Release static  ->  QLADemo-v145-x64-mt-s-1_42_0.exe
cmake --build build\windows-vs2026-x64-static --config Release --target QLADemo

# Debug static    ->  QLADemo-v145-x64-mt-sgd-1_42_0.exe
cmake --build build\windows-vs2026-x64-static --config Debug   --target QLADemo
```

To build only the wrapper library, use `--target QuantLibObjects`. Omit
`--target` to build the default target set (all of the above).

For the VS 2022 presets the toolset tag in the `QLADemo` name is `v143` instead
of `v145`; for the dynamic presets the runtime tag is `-mt` / `-mt-gd` instead
of `-mt-s` / `-mt-sgd` (see section 7).

---

## 7 Output

The build artifacts are written under the preset's build directory
(`build\<preset>\`), keeping them separate from the Visual Studio build's output
(`QuantLibAddin\lib\` and `QuantLibAddin\Clients\Cpp\bin\`) so the two builds do
not overwrite each other.

The `QuantLibObjects` and `AddinCpp` static libraries are intermediate artifacts
produced under the build tree and linked into `QLADemo`. The demo client is the
named, runnable output; its file name encodes the platform toolset, the
platform, the runtime variant and the version, and it is written to
`build\<preset>\cpp\`:

| Preset | Config | Output file (under `build\<preset>\cpp\`) |
|---|---|---|
| `windows-vs2026-x64-static`  | Release | `QLADemo-v145-x64-mt-s-1_42_0.exe` |
| `windows-vs2026-x64-static`  | Debug   | `QLADemo-v145-x64-mt-sgd-1_42_0.exe` |
| `windows-vs2026-x64-dynamic` | Release | `QLADemo-v145-x64-mt-1_42_0.exe` |
| `windows-vs2026-x64-dynamic` | Debug   | `QLADemo-v145-x64-mt-gd-1_42_0.exe` |
| `windows-vs2022-x64-static`  | Release | `QLADemo-v143-x64-mt-s-1_42_0.exe` |
| `windows-vs2022-x64-static`  | Debug   | `QLADemo-v143-x64-mt-sgd-1_42_0.exe` |
| `windows-vs2022-x64-dynamic` | Release | `QLADemo-v143-x64-mt-1_42_0.exe` |
| `windows-vs2022-x64-dynamic` | Debug   | `QLADemo-v143-x64-mt-gd-1_42_0.exe` |

The static-runtime (`-mt-s` / `-mt-sgd`) executable is self-contained and does
not require the Visual C++ runtime to be installed on the target machine.

---

## 8 Troubleshooting

**CMake error: `Could NOT find Boost`** at configure time. `BOOST_INCLUDEDIR`
or `BOOST_LIBRARYDIR` is missing or wrong in your `CMakeUserPresets.json`, or
your Boost build is missing the `filesystem` / `serialization` components.
Re-check section 4: `BOOST_INCLUDEDIR` must point at the folder that contains
the `boost\` sub-directory, and `BOOST_LIBRARYDIR` at the folder that contains
the compiled `.lib` files.

**CMake error: `add_subdirectory given source "QuantLib" which is not an
existing directory`** (or `Cannot find source file` for QuantLib sources) at
configure time. The `QuantLib` folder is missing or misnamed. Clone it as
described in section 3.2 - remember the name is case sensitive.

**CMake error: `No such preset in ...` / `Could not read presets`** when you run
`cmake --preset`. Either you are not in the repository root (run cmake from the
`QuantLibAddin` folder that contains `CMakePresets.json`), or the preset name is
not defined in your `CMakeUserPresets.json`. Check section 4.

**`error C1083: Cannot open include file: 'boost/config.hpp'`** during
compilation. `BOOST_INCLUDEDIR` does not point at the directory that contains
the `boost\` header sub-folder. Fix it in `CMakeUserPresets.json` and
re-configure.

**`error LNK1104: cannot open file 'libboost_...lib'`** (or `...QuantLib...lib`)
at the link stage. The Boost library directory is wrong, or the static/dynamic
runtime variant your preset asked for is not present in `BOOST_LIBRARYDIR`. Make
sure `BOOST_LIBRARYDIR` points at a Boost build that provides the matching
runtime libraries for the preset you chose, then re-configure.

**`error LNK2038: mismatch detected for 'RuntimeLibrary'`** (for example
`value 'MT_StaticRelease' doesn't match value 'MD_DynamicRelease'`), usually
followed by `error LNK1169: one or more multiply defined symbols found`, at the
link stage. Your Boost libraries were built against a different C runtime than
the preset selected. The `dynamic` presets compile `/MD` (dynamic CRT); the
`static` presets compile `/MT` (static CRT), and the Boost `.lib` files in
`BOOST_LIBRARYDIR` must match. A Boost build tags the dynamic-runtime variant
`...-mt-x64-...` (and `...-mt-gd-x64-...` for Debug) and the static-runtime
variant `...-mt-s-x64-...` (and `...-mt-sgd-x64-...`). If `BOOST_LIBRARYDIR`
contains only the `-mt-s` static-runtime libraries, a `dynamic` build links them
anyway, pulling in the static CRT (`libcpmt.lib` / `LIBCMT`) which then collides
with the `/MD` CRT used by the rest of the build. Two fixes:

- **Use the matching preset.** If your Boost libraries are `-mt-s` (static
  runtime), build with a `windows-...-static` preset; if they are `-mt`
  (dynamic runtime), use a `windows-...-dynamic` preset.
- **Build the missing Boost variant.** From your Boost source tree, stage the
  dynamic-runtime libraries alongside the static ones (the different tag keeps
  them side by side):

  ```powershell
  b2 toolset=msvc address-model=64 link=static runtime-link=shared threading=multi ^
     variant=release,debug --with-filesystem --with-serialization stage
  ```

  This produces `libboost_*-vc143-mt-x64-1_83.lib` (and `-mt-gd` for Debug).
  Keep `BOOST_LIBRARYDIR` pointed at `stage\lib` and re-run the dynamic preset.

(The Boost `bind.hpp` "declaring the Bind placeholders ... is deprecated" lines
that may appear earlier in the log are harmless deprecation *messages*, not the
cause of this failure.)

**Python errors during a Full build.**
running a Basic build but expected regeneration. gensrc only runs when you
configure with `-DRUN_GENSRC=ON` (section 5); install Python 3 and ensure
`python --version` works.

---

## 9 Verify the build

`QuantLibObjects` is a static library, so the most direct way to confirm the
build is to run the bundled C++ client, `QLADemo`, which links against it and
exercises a cross-section of the wrapped QuantLib objects.

Run the executable produced in section 7, for example (VS 2026, Release static):

```powershell
build\windows-vs2026-x64-static\cpp\QLADemo-v145-x64-mt-s-1_42_0.exe
```

If it runs to completion and prints its results without error, the QuantLibAddin
layer is good and ready to be consumed by QuantLibXL.

