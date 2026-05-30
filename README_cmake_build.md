# Building QuantLibXL with CMake

This document describes the cmake build for QuantLibXL.  It exists
alongside the hand-maintained Visual Studio solution files documented in
README_vs_build.md; both builds use the same source files and produce
the same XLL output.

---

## 1  Overview

| Property | Value |
|----------|-------|
| Platform | x64 only |
| Compilers | VS 2026 (v145 toolset), VS 2022 (v143 toolset) |
| Configurations | Release static, Debug static, Release dynamic, Debug dynamic |
| XLL output | `build\<preset>\xll\QuantLibXL-<toolset>-x64-mt[-s|-gd|-sgd]-1_23_0.xll` |

The XLL is written into the cmake binary directory (`build\<preset>\xll\`),
which keeps it separate from the output of the hand-maintained solution
files (`QuantLibXL\xll\`).

---

## 2  Prerequisites

The cmake build has the same prerequisites as the hand-maintained build
(see README_vs_build.md sections 2.1 and 2.2) plus:

- **CMake 3.15+** - included with the "C++ CMake tools" component of the
  VS "Desktop development with C++" workload.

---

## 3  Boost paths

Boost paths are not baked into the shared CMakePresets.json.  Instead each
user supplies them in a CMakeUserPresets.json file in the repo root.  That
file is listed in .gitignore and is never committed.

Create CMakeUserPresets.json by copying the template below and adjusting
the paths to match your local Boost installation:

```json
{
    "version": 6,
    "configurePresets": [
        {
            "name": "windows-vs2026-x64-dynamic",
            "displayName": "VS 2026 x64 — dynamic CRT (Release+Debug)",
            "inherits": "windows-vs2026-x64-dynamic-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/path/to/boost/include",
                "BOOST_LIBRARYDIR": "C:/path/to/boost/lib-md"
            }
        },
        {
            "name": "windows-vs2026-x64-static",
            "displayName": "VS 2026 x64 — static CRT (Release+Debug)",
            "inherits": "windows-vs2026-x64-static-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/path/to/boost/include",
                "BOOST_LIBRARYDIR": "C:/path/to/boost/lib-mt"
            }
        },
        {
            "name": "windows-vs2022-x64-dynamic",
            "displayName": "VS 2022 x64 — dynamic CRT (Release+Debug)",
            "inherits": "windows-vs2022-x64-dynamic-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/path/to/boost/include",
                "BOOST_LIBRARYDIR": "C:/path/to/boost/lib-md"
            }
        },
        {
            "name": "windows-vs2022-x64-static",
            "displayName": "VS 2022 x64 — static CRT (Release+Debug)",
            "inherits": "windows-vs2022-x64-static-base",
            "cacheVariables": {
                "BOOST_INCLUDEDIR": "C:/path/to/boost/include",
                "BOOST_LIBRARYDIR": "C:/path/to/boost/lib-mt"
            }
        }
    ],
    "buildPresets": [
        { "name": "windows-vs2026-x64-dynamic-release", "configurePreset": "windows-vs2026-x64-dynamic", "configuration": "Release" },
        { "name": "windows-vs2026-x64-dynamic-debug",   "configurePreset": "windows-vs2026-x64-dynamic", "configuration": "Debug"   },
        { "name": "windows-vs2026-x64-static-release",  "configurePreset": "windows-vs2026-x64-static",  "configuration": "Release" },
        { "name": "windows-vs2026-x64-static-debug",    "configurePreset": "windows-vs2026-x64-static",  "configuration": "Debug"   },
        { "name": "windows-vs2022-x64-dynamic-release", "configurePreset": "windows-vs2022-x64-dynamic", "configuration": "Release" },
        { "name": "windows-vs2022-x64-dynamic-debug",   "configurePreset": "windows-vs2022-x64-dynamic", "configuration": "Debug"   },
        { "name": "windows-vs2022-x64-static-release",  "configurePreset": "windows-vs2022-x64-static",  "configuration": "Release" },
        { "name": "windows-vs2022-x64-static-debug",    "configurePreset": "windows-vs2022-x64-static",  "configuration": "Debug"   }
    ]
}
```

If Boost is installed in a location that cmake can find automatically
(e.g. via BOOST_ROOT or vcpkg), CMakeUserPresets.json can simply be
omitted and the shared presets will work as-is.

---

## 4  Presets

Eight configure presets are provided - one per compiler x CRT combination:

| Configure preset             | Compiler | CRT   |
|------------------------------|----------|-------|
| `windows-vs2026-x64-dynamic` | VS 2026  | `/MD` |
| `windows-vs2026-x64-static`  | VS 2026  | `/MT` |
| `windows-vs2022-x64-dynamic` | VS 2022  | `/MD` |
| `windows-vs2022-x64-static`  | VS 2022  | `/MT` |

Each configure preset has a corresponding pair of build presets with
`-release` and `-debug` suffixes.

---

## 5  Configure

After creating CMakeUserPresets.json (see section 3):

```powershell
# VS 2026, static CRT
cmake --preset windows-vs2026-x64-static -S C:\erik\repos\QuantLibAddin -B C:\erik\repos\QuantLibAddin\build\windows-vs2026-x64-static

# VS 2026, dynamic CRT
cmake --preset windows-vs2026-x64-dynamic -S C:\erik\repos\QuantLibAddin -B C:\erik\repos\QuantLibAddin\build\windows-vs2026-x64-dynamic

# VS 2022, static CRT
cmake --preset windows-vs2022-x64-static -S C:\erik\repos\QuantLibAddin -B C:\erik\repos\QuantLibAddin\build\windows-vs2022-x64-static

# VS 2022, dynamic CRT
cmake --preset windows-vs2022-x64-dynamic -S C:\erik\repos\QuantLibAddin -B C:\erik\repos\QuantLibAddin\build\windows-vs2022-x64-dynamic
```

---

## 6  Build

Pass `--config` and `--target QuantLibXL` to build a specific configuration.
Examples using the VS 2026 static preset:

```powershell
# Release static  ->  QuantLibXL-v145-x64-mt-s-1_23_0.xll
cmake --build C:\erik\repos\QuantLibAddin\build\windows-vs2026-x64-static --config Release --target QuantLibXL

# Debug static    ->  QuantLibXL-v145-x64-mt-sgd-1_23_0.xll
cmake --build C:\erik\repos\QuantLibAddin\build\windows-vs2026-x64-static --config Debug --target QuantLibXL
```

Examples using the VS 2022 static preset:

```powershell
# Release static  ->  QuantLibXL-v143-x64-mt-s-1_23_0.xll
cmake --build C:\erik\repos\QuantLibAddin\build\windows-vs2022-x64-static --config Release --target QuantLibXL

# Debug static    ->  QuantLibXL-v143-x64-mt-sgd-1_23_0.xll
cmake --build C:\erik\repos\QuantLibAddin\build\windows-vs2022-x64-static --config Debug --target QuantLibXL
```

---

## 7  Output locations

| Preset                     | Config  | XLL filename |
|----------------------------|---------|--------------|
| windows-vs2026-x64-static  | Release | `build\windows-vs2026-x64-static\xll\QuantLibXL-v145-x64-mt-s-1_23_0.xll` |
| windows-vs2026-x64-static  | Debug   | `build\windows-vs2026-x64-static\xll\QuantLibXL-v145-x64-mt-sgd-1_23_0.xll` |
| windows-vs2026-x64-dynamic | Release | `build\windows-vs2026-x64-dynamic\xll\QuantLibXL-v145-x64-mt-1_23_0.xll` |
| windows-vs2026-x64-dynamic | Debug   | `build\windows-vs2026-x64-dynamic\xll\QuantLibXL-v145-x64-mt-gd-1_23_0.xll` |
| windows-vs2022-x64-static  | Release | `build\windows-vs2022-x64-static\xll\QuantLibXL-v143-x64-mt-s-1_23_0.xll` |
| windows-vs2022-x64-static  | Debug   | `build\windows-vs2022-x64-static\xll\QuantLibXL-v143-x64-mt-sgd-1_23_0.xll` |
| windows-vs2022-x64-dynamic | Release | `build\windows-vs2022-x64-dynamic\xll\QuantLibXL-v143-x64-mt-1_23_0.xll` |
| windows-vs2022-x64-dynamic | Debug   | `build\windows-vs2022-x64-dynamic\xll\QuantLibXL-v143-x64-mt-gd-1_23_0.xll` |

---

## 8  cmake file layout

```
CMakeLists.txt              <- root: wires together all subprojects
CMakePresets.json           <- configure and build presets
cmake\
  commonSettings.cmake      <- MSVC compile options, CRT selection, auto-link suppressors
ObjectHandler\
  CMakeLists.txt            <- builds xlsdk, ohlib, ohxllib static libs
QuantLibAddin\
  CMakeLists.txt            <- builds QuantLibObjects static lib
QuantLibXL\
  CMakeLists.txt            <- builds the XLL
QuantLib\
  CMakeLists.txt            <- upstream QuantLib cmake (unchanged)
```

---

## 9  Design notes

### Auto-link suppression

The ObjectHandler and QuantLibAddin sources include Boost-style auto-link
headers (`oh/auto_link.hpp`, `qlo/auto_link.hpp`, `xlsdk/auto_link.hpp`)
that emit `#pragma comment(lib, ...)` directives referencing tagged library
names (e.g. `QuantLibObjects-v145-x64-mt-s-1_23_0.lib`).  These conflict
with the cmake-managed link step.

Each auto_link header has been guarded with a `#ifndef` macro:

| Header                | Guard macro            |
|-----------------------|------------------------|
| `oh/auto_link.hpp`    | `OH_NO_AUTO_LINK`      |
| `qlo/auto_link.hpp`   | `QLADDIN_NO_AUTO_LINK` |
| `xlsdk/auto_link.hpp` | `XLSDK_NO_AUTO_LINK`   |

`cmake/commonSettings.cmake` defines all three macros so the cmake build
never uses the auto-link mechanism.  The hand-maintained solution files do
not define these macros, so their behaviour is unchanged.

### QuantLib tagged layout

QuantLib's cmake `QL_TAGGED_LAYOUT` option, when ON, sets
`CMAKE_RELEASE_POSTFIX` and `CMAKE_DEBUG_POSTFIX` globally, which would
apply the tagged suffix to every cmake target in the build.  To avoid
this, `QL_TAGGED_LAYOUT` is set to OFF in the root CMakeLists.txt and the
postfix is applied only to `ql_library` via `set_target_properties`.

### XLL output name

The XLL `OUTPUT_NAME` uses cmake generator expressions to select the
correct runtime tag per configuration:

```
QuantLibXL-<toolset>-x64-<runtime-tag>-1_23_0.xll
```

`<toolset>` is derived from `MSVC_TOOLSET_VERSION` at configure time
(e.g. `v145` for VS 2026, `v143` for VS 2022).  `<runtime-tag>` is
`-mt-s` / `-mt-sgd` (static CRT) or `-mt` / `-mt-gd` (dynamic CRT)
depending on `MSVC_LINK_DYNAMIC_RUNTIME`.
