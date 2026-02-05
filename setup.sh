#!/bin/bash

APP_NAME="xclock_tiny.desktop"
TARGET_DIR="$HOME/.local/share/applications"
CURRENT_DIR=$(cd "$(dirname "$0")" && pwd)

case "$1" in
    install)
        echo "Install..."
        cp "$APP_NAME" "$APP_NAME.tmp"
        
        sed -i "s|Exec=.*|Exec=$CURRENT_DIR/xclock_tiny|" "$APP_NAME.tmp"
        sed -i "s|Icon=.*|Icon=$CURRENT_DIR/xclock_tiny.png|" "$APP_NAME.tmp"
        
        mkdir -p "$TARGET_DIR"
        mv "$APP_NAME.tmp" "$TARGET_DIR/$APP_NAME"
        chmod +x "$TARGET_DIR/$APP_NAME"
        
        update-desktop-database "$TARGET_DIR" 2>/dev/null
        echo "Installed to $TARGET_DIR"
        ;;
    uninstall)
        echo "Uninstall..."
        rm -f "$TARGET_DIR/$APP_NAME"
        update-desktop-database "$TARGET_DIR" 2>/dev/null
        echo "Uninstalled."
        ;;
    *)
        echo "Usage: $0 {install|uninstall}"
        exit 1
        ;;
esac
