# QuantLibAddin Build Configurations

---

## Hand-maintained Visual Studio solution files

Both solutions are opened in Visual Studio or built with MSBuild directly.
The output XLL is written to `QuantLibXL\xll\`.

### QuantLibXL\QuantLibXL_basic.sln

Compiles pre-existing auto-generated source files. Does not require Python or gensrc.

| Configuration             | Platform | Runtime      | Output filename                          |
|---------------------------|----------|--------------|------------------------------------------|
| Release                   | x64      | Dynamic (/MD)  | `QuantLibXL-v145-x64-mt-1_21_0.xll`     |
| Release (static runtime)  | x64      | Static (/MT)   | `QuantLibXL-v145-x64-mt-s-1_21_0.xll`   |
| Debug                     | x64      | Dynamic (/MDd) | `QuantLibXL-v145-x64-mt-gd-1_21_0.xll`  |
| Debug (static runtime)    | x64      | Static (/MTd)  | `QuantLibXL-v145-x64-mt-sgd-1_21_0.xll` |
| Release                   | Win32    | Dynamic (/MD)  | `QuantLibXL-v145-mt-1_21_0.xll`         |
| Release (static runtime)  | Win32    | Static (/MT)   | `QuantLibXL-v145-mt-s-1_21_0.xll`       |
| Debug                     | Win32    | Dynamic (/MDd) | `QuantLibXL-v145-mt-gd-1_21_0.xll`      |
| Debug (static runtime)    | Win32    | Static (/MTd)  | `QuantLibXL-v145-mt-sgd-1_21_0.xll`     |

### QuantLibXL\QuantLibXL_full.sln

Runs gensrc first to regenerate all auto-generated source files, then compiles.
Requires Python 3. Same 8 configuration/platform combinations as the basic solution,
with identical output naming.

> **Note:** The Win32 configurations exist in both solutions but are not practically
> useful — QuantLib dropped 32-bit support and the build targets x64 only in practice.
> Only the x64 configurations have been tested.

