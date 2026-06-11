# Visual Studio Installations and Build Compatibility

---

## Visual Studio installations on this machine

| Installation path                              | Product                           | VS Version | MSBuild version | MSVC toolset | Platform toolset tag |
|------------------------------------------------|-----------------------------------|------------|-----------------|--------------|----------------------|
| `C:\Program Files\Microsoft Visual Studio\18\` | Visual Studio 2026 Professional   | 18.4.0     | 18.4.0          | 14.50.35717  | `v145`               |
| `C:\Program Files\Microsoft Visual Studio\2022\`| Visual Studio 2022 Professional  | 17.14.28   | 17.14.40        | 14.44.35207  | `v143`               |

A `2017` entry appears in the parent directory listing but the folder does not
exist — it is a stale artefact from the VS Installer, not a real installation.

---

## Which VS version is used by each build configuration

### Hand-maintained solution files

The platform toolset is selected automatically by `QuantLib\QuantLib.props`,
which maps the `VisualStudioVersion` environment variable to a toolset tag:

| VisualStudioVersion | Platform toolset | Used when building with |
|---------------------|------------------|-------------------------|
| `17.0`              | `v143`           | VS 2022 MSBuild         |
| `18.0`              | `v145`           | VS 2026 MSBuild         |

Both installations produce a working build. The output filename differs:
- Built with VS 2026: `QuantLibXL-v145-x64-mt-s-1_21_0.xll`
- Built with VS 2022: `QuantLibXL-v143-x64-mt-s-1_21_0.xll`

When the solution is opened in VS 2026 (the normal workflow), `VisualStudioVersion`
is `18.0` and the `v145` toolset is used automatically.

