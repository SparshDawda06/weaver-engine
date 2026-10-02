#!/bin/bash
# scripts/start_backend.sh - Starts or restarts the Weaver inference engine backend in tmux
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
STRATA_DIR="/home/sparsh/strata-app"

echo "=== Weaver Inference Engine: Starting Backend ==="

if ! command -v tmux &> /dev/null; then
    echo "Error: tmux is required to run the backend as a detached daemon."
    exit 1
fi

# Check if already running and healthy
if curl -s http://127.0.0.1:8080/v1/models > /dev/null 2>&1; then
    echo "Backend is already running and healthy on port 8080."
    echo "Run './build/weaver -s' to see live telemetry."
    exit 0
fi

echo "Restarting backend daemon in tmux session 'weaver_server'..."
tmux kill-session -t weaver_server 2>/dev/null || true
tmux new-session -d -s weaver_server "cd $STRATA_DIR && ./run-iq3_xxs.sh"

echo "Waiting for backend to initialize (loading 40GB model shards)..."
for i in {1..40}; do
    if curl -s http://127.0.0.1:8080/v1/models > /dev/null 2>&1; then
        echo "Backend successfully initialized and serving on http://127.0.0.1:8080"
        exit 0
    fi
    sleep 2
    printf "."
done

echo ""
echo "Backend is still initializing. Check status with: tmux capture-pane -pt weaver_server"
