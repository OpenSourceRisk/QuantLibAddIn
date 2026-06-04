# Installing software on this machine (WSL + Windows) — agent field notes

This document records how doxygen and Graphviz (`dot`) were installed for use under
WSL on this machine, and — more importantly — the environment-specific constraints that
make software installation here unusual. It is written for future instances of the agent
so they do not have to rediscover these gotchas.

## TL;DR

- **No root.** `sudo` exists but is permission-denied. Install everything in user space.
- **Corporate firewall (Zscaler) filters by destination host.** GitHub's download CDN
  works; GitLab's package CDN (which redirects to Google Cloud Storage) is blocked and
  silently returns an HTML block page instead of your file.
- **`/mnt/c` is read-only** from inside WSL. Do work under `/home/developer`.
- **`~/.bashrc` is immutable** (`chattr +i`). You cannot edit it without root. Use
  `~/.bash_aliases` (interactive shells) and `~/.bash_profile` (login shells) instead.
- Prefer **prebuilt binaries** over compiling, and prefer **GitHub-hosted** downloads.

---

## doxygen (WSL)

doxygen has a large dependency chain if installed from the distro `.deb`
(clang/LLVM, xapian, etc.). The official prebuilt Linux binary from doxygen.nl avoids
all of that — its only shared-library needs (libc, libstdc++, libgcc_s, zlib, etc.) are
already satisfied on Ubuntu 22.04.

Steps used:

1. Download the official Linux binary tarball (doxygen 1.9.8) into user space:
   `~/tools/doxygen-1.9.8/`.
2. Verify: `~/tools/doxygen-1.9.8/bin/doxygen --version` -> `1.9.8 (...)`.
3. Smoke test: generated HTML from a tiny header; confirmed `html/index.html` produced.

No root, no apt, no compilation required.

## Graphviz / `dot` (WSL)

`dot` was **not** available as a single self-contained binary, so it was assembled from
Ubuntu `.deb` packages extracted into user space (no install, no root):

1. `apt-get download graphviz libgvc6 libcgraph6 libgvpr2 libcdt5 libpathplan4 \
   libxdot4 liblab-gamut1 libgts-0.7-5 libann0` (plus pango/cairo/freetype/fontconfig
   and their runtime deps). `apt-get download` works here even though `sudo apt install`
   does not.
2. Extract each `.deb` with `dpkg-deb -x <pkg>.deb ~/tools/graphviz`. This unpacks the
   files without needing root or running maintainer scripts.
3. Point the runtime at the user-space prefix:
   - `PATH` += `~/tools/graphviz/usr/bin`
   - `LD_LIBRARY_PATH` += the graphviz plugin dir and lib dir
   - `GVBINDIR` = the graphviz plugin dir
4. **Register the plugin database once:** `dot -c`. This writes a `config6a` file next to
   the plugin `.so` files. Without it, `dot` cannot find its layout engines and fails with
   "no plugin for ... layout". (On Windows the equivalent is `dot.exe -c`.)
5. Verify: `dot -V` -> `graphviz version 2.43.0`; rendered a test graph to PNG and SVG.
6. Combined test: ran doxygen with `HAVE_DOT = YES` over a small class hierarchy and
   confirmed it drove `dot` to emit inheritance/collaboration diagrams (SVG).

### Dependency-closure tip

When extracting `.deb`s piecemeal, run `ldd` on the plugin `.so` files to find missing
shared libraries (e.g. `libltdl.so.7 => not found`), then `apt-get download` + extract the
package that provides them. Repeat until `ldd` is clean. Some packages reported by `ldd`
are already present as base-system libs, so a 404 on `apt-get download` for one of them is
often harmless.

---

## Making the tools available in every shell

`~/.bashrc` here has the **immutable attribute** set:

```
$ lsattr ~/.bashrc
----i---------e------- /home/developer/.bashrc
```

That means even the owner cannot modify it (`touch`/append -> "Operation not permitted");
clearing the flag needs `chattr -i`, which needs root. Workarounds that DO work:

- **Interactive shells:** the stock Ubuntu `.bashrc` already contains
  `if [ -f ~/.bash_aliases ]; then . ~/.bash_aliases; fi`. `~/.bash_aliases` does not exist
  by default and is creatable, so put your `export PATH=...` block there.
- **Login shells:** `~/.bash_profile` is writable (not immutable) and does not source
  `.bashrc`, so append the same block there too.

The block exported:

```sh
export PATH=/home/developer/tools/doxygen-1.9.8/bin:/home/developer/tools/graphviz/usr/bin:$PATH
export LD_LIBRARY_PATH=/home/developer/tools/graphviz/usr/lib/x86_64-linux-gnu/graphviz:/home/developer/tools/graphviz/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
export GVBINDIR=/home/developer/tools/graphviz/usr/lib/x86_64-linux-gnu/graphviz
```

---

## The corporate firewall (Zscaler) — what to expect

This machine's internet access goes through **Zscaler**, a corporate "secure web gateway"
(a filtering HTTPS proxy). It enforces an allow/block policy **by destination host**.

Key terms:

- **CDN (Content Delivery Network):** a set of distributed edge servers that host download
  files so they are served quickly from a nearby location. Release/download assets on
  GitHub and GitLab are served from CDNs, not from the web UI host.
- **GCS (Google Cloud Storage):** Google's cloud file-hosting service (HTTPS "buckets").
  GitLab's generic **package registry redirects downloads to GCS** (`storage.googleapis.com`).

What was observed:

- **GitHub release/asset CDN: ALLOWED.** The doxygen Windows ZIP and Linux binary
  downloaded fine from `github.com` release URLs.
- **GitLab package CDN (-> Google Cloud Storage): BLOCKED.** Downloading the Graphviz
  Windows ZIP from GitLab's package registry returned a **Zscaler block page** instead of
  the file. The give-aways:
  - The "download" was tiny (~15.8 KB) and never grew.
  - Its first bytes were HTML, not the ZIP magic `PK\x03\x04`:
	```
	<!--# 3rtjtJf4M8qJqDRQP8RV3J0jRj3rH4tJ5SQfFPjd-->
	<!DOCTYPE HTML ...>
	<meta name="description" content="Zscaler makes the internet safe ...">
	```

### Practical rules for downloading here

1. **Prefer GitHub-hosted artifacts.** They went through reliably.
2. **Distrust suspiciously small downloads.** If a multi-MB file arrives as a few KB,
   inspect the first bytes. A real ZIP starts with `PK` (`50 4B`); a real gzip with
   `1F 8B`. HTML / a Zscaler meta tag means you hit the block page.
3. **Connection resets happen mid-download** even on allowed hosts. `curl.exe -C -`
   (resume) in a short retry loop reliably finishes large files — BUT only loop while the
   file size keeps growing. If the size is stuck (as with a block page), looping is
   pointless; stop and switch source/host.
4. **`apt-get download` works** for fetching `.deb`s (it uses the archive mirrors), even
   though `sudo apt install` does not (no root).
5. **No proxy env vars are set** (`HTTP_PROXY`/`HTTPS_PROXY` are empty); Zscaler is
   transparent/system-level, so you cannot route around it by unsetting a variable.

### winget caveat

`winget show` / `winget install` can **hang indefinitely** in this non-interactive
terminal (it blocks on a "source agreements" prompt or a slow source query). Avoid winget
in automated/agent contexts; if you must, expect to have to kill the process.

---

## Driving WSL from the Windows (PowerShell) agent

- Long inline `wsl.exe -- bash -c '...'` commands are fragile (PowerShell quoting + an
  intermittent security agent that blocks long command lines). The reliable pattern is:
  write the script to a Windows file, base64-encode it, pipe it into WSL, decode, strip
  CRs, and run:
  ```powershell
  $b = [Convert]::ToBase64String([IO.File]::ReadAllBytes("C:\path\script.sh"))
  wsl.exe -- bash -c "echo $b | base64 -d | tr -d '\r' > /home/developer/script.sh; bash /home/developer/script.sh"
  ```
- Suppress PowerShell's progress UI for downloads (`$ProgressPreference='SilentlyContinue'`)
  or use `curl.exe`, otherwise the progress redraw floods the captured output.

---

## Summary of where things live (WSL)

| Tool    | Path                                                        | Version |
|---------|------------------------------------------------------------|---------|
| doxygen | `/home/developer/tools/doxygen-1.9.8/bin/doxygen`          | 1.9.8   |
| dot     | `/home/developer/tools/graphviz/usr/bin/dot`               | 2.43.0  |

Both are on `PATH` for new shells via `~/.bash_aliases` and `~/.bash_profile`.
