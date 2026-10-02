#!/bin/bash
# scripts/run_weaver.sh - Easy launcher for the Weaver Engine CLI
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Ensure backend daemon is active
if ! curl -s http://127.0.0.1:8080/v1/models > /dev/null 2>&1; then
    echo "[!] Weaver backend is not responding on http://127.0.0.1:8080."
    echo "[*] Auto-starting backend..."
    "$DIR/scripts/start_backend.sh"
fi

# Ensure binary is built
if [ ! -f "$DIR/build/weaver" ]; then
    echo "[*] Building weaver binary..."
    mkdir -p "$DIR/build"
    (cd "$DIR/build" && cmake .. >/dev/null && make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4) >/dev/null)
fi

# If no arguments provided, launch interactive chat mode
if [ $# -eq 0 ]; then
    exec "$DIR/build/weaver" -i
else
    exec "$DIR/build/weaver" "$@"
fi
