# Linux and SteamOS builds

Moonlight-Switch uses the same SDL3 Linux implementation on SteamOS and other
desktop distributions. There is no separate SteamOS input, rendering, or
streaming path.

## Choosing a download

Every release is built for both `x86_64` and `aarch64`.

| System | Recommended artifact |
| --- | --- |
| Steam Deck / SteamOS | `steamrt4-x86_64.tar.gz` |
| Ubuntu, Debian, Linux Mint | `amd64.deb` or `arm64.deb` |
| Fedora | `x86_64.rpm` or `aarch64.rpm` |
| Arch, Manjaro, openSUSE, and other desktop distributions | AppImage |
| Systems without AppImage/FUSE support | Portable `linux-*.tar.gz` |
| ARM64 laptops and Raspberry Pi-class desktops | `aarch64` AppImage/tar or `arm64.deb` |

Steam Deck is an x86_64 device. The ARM64 Steam Runtime artifact is provided for
other ARM64 Linux gaming systems.

The AppImage, portable tarball, DEB, and RPM contain private copies of
application libraries under the application directory. They intentionally do
not bundle OpenGL/EGL, Mesa, libdrm, libva, VDPAU, or GPU video drivers. Those
components must come from the host so hardware decoding remains compatible with
the installed GPU driver. The vendor-neutral OpenCL ICD loader is bundled when
the selected FFmpeg build requires it, but the OpenCL implementation and GPU
driver still come from the host.

## Installing

### SteamOS / Steam Deck

Extract the Steam Runtime archive:

```bash
tar -xzf Moonlight-Switch-*-steamrt4-*.tar.gz
```

In Steam Desktop Mode, open `Moonlight-Switch.AppDir` in the file manager and
double-click **Add-to-Steam.desktop**. Confirm **Execute** if the desktop asks
for permission. The helper uses SteamOS's shortcut command when available, then
falls back to the registered Steam URI handler, native Steam, or Flatpak Steam.
It does not require a keyboard, root access, or the Steam file picker. Return to
Game Mode and find Moonlight-Switch under **Library → Non-Steam**.

If the helper is unavailable on a non-SteamOS distribution, select
**Games → Add a Non-Steam Game**, browse to
`Moonlight-Switch.AppDir/AppRun`, and add it manually.

For a Steamworks depot, upload the *contents* of `Moonlight-Switch.AppDir`, use
`AppRun` as the Linux launch executable, and select **Steam Linux Runtime 4.0**
in the Steamworks Linux Runtime settings.

### AppImage

```bash
chmod +x Moonlight-Switch-*-linux-x86_64.AppImage
./Moonlight-Switch-*-linux-x86_64.AppImage
```

Use the `aarch64` filename on ARM64. If FUSE is unavailable, AppImage also
supports:

```bash
./Moonlight-Switch-*.AppImage --appimage-extract-and-run
```

### Debian, Ubuntu, and Linux Mint

```bash
sudo apt install ./moonlight-switch_*_amd64.deb
```

Use the `arm64.deb` file on ARM64.

### Fedora

```bash
sudo dnf install ./moonlight-switch-*.x86_64.rpm
```

Use the `aarch64.rpm` file on ARM64. On RPM distributions with different
dependency naming, use the AppImage instead.

### Portable tarball

```bash
tar -xzf Moonlight-Switch-*-linux-x86_64.tar.gz
./Moonlight-Switch.AppDir/AppRun
```

## Building locally

The Linux release preset requires CMake 3.23 or newer, Ninja, a C++20 compiler,
and development packages for FFmpeg, curl, Mbed TLS, Jansson, PNG, Opus, Expat,
zstd, EGL/OpenGL ES, SDL's Linux backends, and libva.

Configure and build:

```bash
cmake --preset linux-release
cmake --build --preset linux-release --parallel
```

The release preset embeds resources and verifies at configure time that the
selected FFmpeg libraries expose VA-API hardware configurations for both H.264
and HEVC. A developer can explicitly disable that requirement with
`-DMOONLIGHT_LINUX_REQUIRE_VAAPI=OFF`, but release artifacts must leave it
enabled.

Create every portable package:

```bash
scripts/package-linux.sh \
  --build-dir build/linux-release \
  --dist-dir build/linux-dist \
  --channel portable
```

The packaging script downloads the architecture-matched `linuxdeploy` tool,
stages private libraries, verifies that no host graphics libraries were
bundled, and produces AppImage, tar, DEB, and RPM files.

## GitHub Actions

`.github/workflows/linux.yml` has four native matrix entries:

- Ubuntu 22.04 portable x86_64
- Ubuntu 22.04 portable ARM64
- Steam Linux Runtime 4 SDK x86_64
- Steam Linux Runtime 4 SDK ARM64

The portable builds use Ubuntu 22.04 as a conservative glibc baseline. The
Steam builds use Valve's official Steam Runtime 4 SDK images. No architecture
is emulated or cross-compiled, so the resource generator and all configure-time
hardware-decoder checks run as native executables.

The workflow is called by `all-builds.yml`; each Linux format is uploaded as a
separate downloadable workflow artifact.

## Hardware decoding validation

Configuration verifies FFmpeg support, but only a machine with a GPU can test
the driver. On a Steam Deck or Linux desktop:

1. Enable **Use hardware decoding** in Moonlight-Switch.
2. Start an H.264 or HEVC stream.
3. Inspect `log.log` in the application data directory.
4. Confirm it reports an initialized Linux `vaapi` hardware decoder rather
   than a software-decoding fallback.

The package must be able to access the host render node, normally
`/dev/dri/renderD128`.
