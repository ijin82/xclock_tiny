# XClock Tiny

Tiny clock app for GNOME (tested on 48 version, wayland session).

It is designed to display the clock on top of other windows when the main panel is hidden or inaccessible.  
It is convenient for use with two or more monitors, as a system clock on top of games.

<img width="995" height="598" alt="Screenshot From 2026-05-12 12-58-22" src="https://github.com/user-attachments/assets/6a1bacc2-77a7-4e8c-975a-caa84c09e78c" />

## Build & Install

```bash
cd ~/your-folder-name

git clone [this repo] .

sudo apt-get install libX11-devel libXft-devel libXext-devel gcc

gcc xclock_tiny.c -o xclock_tiny -lX11 -lXft -lXext -I/usr/include/freetype2

chmod +x setup.sh

./setup.sh install # to install app icon

./setup.sh uninstall # to uninstall app icon
```

## Info

- Memory usage about 1Mb  
- Right click - exit
- Left  click - drag window
- Alt+Space - call system menu to set up "Always on top" (because of the security issues it is not possible to enable "always on top" by default for now)

## Useful
It is convenient to use GNOME **Extension Alt-Tab Hide** to exclude this window from Alt-Tab queue

**No configuration included**, but for now you can easily fix the code by yourself, it's quite simple.

## Plans

- Configuration options
- Flathub submission
