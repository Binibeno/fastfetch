# Fastfetch on iOS (Jailbroken iPhone 8) — Working Notes

_Last updated: 2025-10-25_

## Environment
- Device: iPhone 8 (model `iPhone10,4`) running iOS 16.0 kernel (`Darwin 22.6.0`).
- Access: jailbroken user `mobile` via SSH (`ssh -tt mobile@iphone-8`).
- Package manager: Procursus `apt` (sudo password on device appears to be `alpine`).
- Build toolchain installed on device: `clang 16`, `cmake 3.30.2`, `meson 0.64.0`, `ninja 1.10.2`, `pkg-config 0.29.2`, `python3 3.9.9`.
- Host-side convenience: mounted phone root at `/home/fasz32/iphonefs` using `sshfs mobile@iphone-8:/ /home/fasz32/iphonefs -o reconnect,ServerAliveInterval=15,ServerAliveCountMax=3`.

## Repository State
- Location: `/var/mobile/fastfetch` (mirrored locally at `/home/fasz32/iphonefs/var/mobile/fastfetch`).
- Branch: upstream `dev` (cloned with `git clone --depth 1 https://github.com/fastfetch-cli/fastfetch.git`).
- Local changes (as of last edit):
  - `CMakeLists.txt` modified to detect iOS builds and disable unavailable subsystems before the platform-specific source selection.
  - `src/options/general.c` now refuses `general.preRun` on iOS (no `system()`), includes `TargetConditionals`.
  - `src/common/processing_linux.c` guards macOS-only headers/APIs with `TARGET_OS_IPHONE` checks.
  - New stub file `src/common/netif/netif_ios.c` that returns `false` for default-route queries.
  - Build directory `build-ios/` (generated via `cmake -S . -B build-ios ...`).

## Build Attempts Summary
1. **Initial meson attempt** — failed because upstream uses CMake; installed `cmake` instead.
2. **First CMake configure** (`cmake -S . -B build-ios -DCMAKE_BUILD_TYPE=Release`): failed during compilation because iOS SDK lacks `OpenCL/cl.h` & `OpenGL/gl.h`.
3. **Mitigation**: introduced `FF_IOS` flag to disable OpenCL/OpenGL/Vulkan/EGL/GLX at configure time, and removed their sources from `LIBFASTFETCH_SRC` for iOS.
4. **Second build**: proceeded further but hit `system()` unavailability and `IOKit/kext/KextManager.h` include errors. Added guards + removed `kmod`.
5. **Third build**: blocked by missing `net/route.h` (not in iPhone SDK). Stubbed netif implementation for iOS.
6. **Fourth build**: blocked by `IOBluetooth` and `IOKit/pwr_mgt` headers, then by CoreGraphics display types — these frameworks simply do not exist on iOS.

## Repeated Obstacles
- **Framework availability**: The iPhoneOS SDK shipped with Procursus lacks desktop macOS frameworks that Fastfetch’s Apple backend requires (CoreWLAN, IOBluetooth, OpenGL, OpenCL, IOKit power headers, CoreDisplay/DisplayServices, etc.). Even weak-linking is impossible because the headers/libs are missing entirely.
- **Header availability**: Several POSIX extensions that Fastfetch expects (`wordexp`, `sys/user.h`, `net/route.h`) are marked unavailable on iOS.
- **Feature breadth**: The `elseif(APPLE)` branch in `CMakeLists.txt` appends ~60 macOS-specific source files. Almost all of them assume macOS-only frameworks.
- **Upstream support matrix**: README lists Linux/macOS/Windows/BSD/Android/Haiku/SunOS. iOS is not a supported target at all, so there is no existing abstraction for its Sandbox/framework model.

## Current Code Changes (High Level)
- `CMakeLists.txt`
  - Defines `FF_IOS` when `APPLE` and the normalized `CMAKE_SYSTEM_PROCESSOR` matches `iphone|ipad|ipod`.
  - Forces `ENABLE_{VULKAN,OPENCL,EGL,GLX,WORDEXP}` off under iOS.
  - For the Apple target, conditionally replaces OpenCL/OpenGL/Vulkan modules and `kmod`/`netif_apple` with stubs when `FF_IOS` is true; temporarily also swapping in `_nosupport` versions for battery/bluetooth modules.
- `src/options/general.c`
  - Adds `#include <TargetConditionals.h>` and returns “general.preRun is not supported on iOS” instead of calling `system()`.
- `src/common/processing_linux.c`
  - Wraps macOS-specific includes (`sys/user.h`, `libproc.h`, sysctl code paths) with `TARGET_OS_IPHONE` guards, falling back to Linux logic when unavailable.
- `src/common/netif/netif_ios.c`
  - Stub functions so the build doesn’t depend on `net/route.h`.

## Remaining Work to Achieve a Build
1. **Strip or replace every macOS-only detection module**:
   - Battery (`src/detection/battery/battery_apple.c`), brightness, bluetooth, board, CPU, GPU, display server, packages, wallpaper, wifi, etc. almost all rely on frameworks missing on iOS.
   - Need either new iOS-specific detectors (e.g., leveraging private frameworks like `SpringBoardServices`, `BackBoardServices`, `MobileGestalt`, `IOKit` classes actually present on iOS) or fallback `_nosupport` implementations for each module referenced in `LIBFASTFETCH_SRC`.
2. **Audit utility helpers**:
   - Files under `src/util/apple/` (e.g., `cf_helpers.c`, `osascript.m`, `smc_temps.c`) expect CoreFoundation + SMC interfaces that are macOS-only.
   - Need to exclude or replace them for iOS.
3. **Package manager detection**:
   - iOS uses Procursus/Sileo (dpkg). Ensure `PACKAGES_DISABLE_*` or new detectors don’t assume `brew/macports`.
4. **Testing**: Once the code compiles, need to run `fastfetch` on-device and verify modules degrade gracefully.

## Commands Used Frequently
```bash
ssh -tt mobile@iphone-8 'pwd'
ssh -tt mobile@iphone-8 'printf "alpine\n" | sudo -S apt-get install -y cmake meson ninja pkg-config'
ssh -tt mobile@iphone-8 'cd /var/mobile/fastfetch && cmake -S . -B build-ios -DCMAKE_BUILD_TYPE=Release -DENABLE_VULKAN=OFF -DENABLE_OPENCL=OFF -DENABLE_EGL=OFF -DENABLE_GLX=OFF -DENABLE_WORDEXP=OFF'
ssh -tt mobile@iphone-8 'cd /var/mobile/fastfetch && cmake --build build-ios -j4'
sshfs mobile@iphone-8:/ /home/fasz32/iphonefs -o reconnect,ServerAliveInterval=15,ServerAliveCountMax=3
```

## Suggested Next Steps if You Resume
1. **Decide on scope**: either keep replacing modules with `_nosupport` to get *any* binary, or invest in actual iOS detections (significant reverse-engineering effort).
2. **Modularize Apple sources**: Introduce a fresh `elseif(FF_IOS)` block in `CMakeLists.txt` with its own curated source list instead of reusing the macOS list.
3. **Identify available iOS frameworks**: Use `ls /System/Library/Frameworks` & `/System/Library/PrivateFrameworks` to see what’s actually present; focus on APIs like `MobileGestalt`, `IOKit` classes that exist on iOS, `libMobileGestalt.dylib`, `libproc` (if any), etc.
4. **Consider alternative tools**: If the goal is just a fetch output on device, `neofetch`/`pfetch`/`mobilefetch` from Procursus already run without huge porting work.
5. **Coordinate with upstream**: The fastfetch maintainers may prefer a clean separation (e.g., `src/detection/*_ios.c`) and might offer guidance on expected abstractions.

## Miscellaneous Notes
- Upstream README: <https://github.com/fastfetch-cli/fastfetch> — no mention of iOS support.
- Building requires many `sudo` invocations on-device; the Procursus environment is relatively slow, so keep command timeouts high (`timeout_ms ≈ 180000`).
- When running `cmake --build`, expect repeated reconfiguration because the repo lacks tags (shallow clone) and `fatal: No names found` warnings appear — harmless but noisy.
- `rg` is not installed on-device; rely on `grep`/`sed` or install ripgrep via Procursus if needed.

---
Feel free to expand this file with future findings so you (or someone else) can pick up the iOS port later.
