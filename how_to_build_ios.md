# Building Fastfetch for Jailbroken iOS (iPhone 8)

These steps reproduce the working iOS build we prepared. They assume:
- This Linux host (Ubuntu 22.04) has the repo mounted at `/home/fasz32/iphonefs/private/var/mobile/fastfetch` via SSHFS.
- You can SSH into the phone as `mobile` (password `alpine`).

## 1. Ensure Remote Toolchain Is Installed
On the iPhone (over SSH):

```bash
# Update package metadata
printf 'alpine\n' | sudo -S apt-get update

# Install build tools
printf 'alpine\n' | sudo -S apt-get install -y \
  clang cmake ninja pkg-config git python3 ldid meson
```

### Optional: install odcctools for otool
```bash
printf 'alpine\n' | sudo -S apt-get install -y odcctools
```

## 2. Clone / Update Fastfetch
Already cloned at `/var/mobile/fastfetch`. To refresh to upstream dev:

```bash
cd /var/mobile/fastfetch
# pull latest changes if desired
# git pull origin dev
```

Local workstation has the same tree at `/home/fasz32/iphonefs/private/var/mobile/fastfetch` thanks to SSHFS.

## 3. Configure CMake Build
From the Linux host (or via SSH):

```bash
ssh -tt mobile@iphone-8 <<'CMD'
cd /var/mobile/fastfetch
cmake -S . -B build-ios \
  -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_VULKAN=OFF \
  -DENABLE_OPENCL=OFF \
  -DENABLE_EGL=OFF \
  -DENABLE_GLX=OFF \
  -DENABLE_WORDEXP=OFF
CMD
```

## 4. Build
Use single-threaded build (device has limited resources):

```bash
ssh -tt mobile@iphone-8 <<'CMD'
cd /var/mobile/fastfetch
cmake --build build-ios -j1
CMD
```

## 5. Sign Binaries
Before running, sign `fastfetch` and `flashfetch` with entitlements:

```bash
ssh -tt mobile@iphone-8 <<'CMD'
cd /var/mobile/fastfetch/build-ios
cat > fastfetch.entitlements <<'plist'
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>platform-application</key><true/>
  <key>com.apple.private.security.container-required</key><false/>
  <key>com.apple.private.skip-library-validation</key><true/>
</dict>
</plist>
plist

ldid -Sfastfetch.entitlements fastfetch
ldid -Sfastfetch.entitlements flashfetch
CMD
```

Repeat the `ldid` commands after every rebuild.

## 6. Run / Test
```bash
ssh -tt mobile@iphone-8 '/var/mobile/fastfetch/build-ios/fastfetch'
ssh -tt mobile@iphone-8 '/var/mobile/fastfetch/build-ios/flashfetch'
```

Expected default output (abbreviated):
- Apple ASCII logo
- OS / Host / Kernel / Uptime / Packages
- Shell, Terminal
- CPU, Memory, Disk usage

## 7. Troubleshooting
- If output shows “Killed: 9”, resign binaries with `ldid`.
- If uptime is wrong, ensure `build-ios` uses latest source (`uptime_nosupport.c` must return milliseconds).
- If compilation fails due to missing headers, verify guards for `TargetConditionals.h` and availability of frameworks are up to date.

## Working Tree Notes
Major iOS-specific changes are documented in `IOS_PORT_NOTES.md` inside the repo; consult that if updating modules.
