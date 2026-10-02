# Weaver Engine

High-performance hybrid GPU/CPU MoE inference engine and client, optimized for modern large-scale Mixture-of-Experts architectures (specifically **Qwen3.8-Flash-Next 125B** with 512 experts/layer, top-10 routed + 1 shared expert) with a full **128K context window (`131072`)**.

---

## ⚡ Highlights & Optimizations

- **Physical Core Affinity & SMT Isolation**: Patched core threadpool to pin directly to physical core IDs via `/sys/devices/system/cpu/cpu*/topology/core_id`, isolating Core 0 for host coordination and eliminating hyperthreading/SMT port stalls.
- **VRAM Expert Caching**: Dynamic VRAM budgeting reserving 450 MiB for KV allocation and CUDA workspace, fitting resident experts directly in GPU VRAM alongside offloaded CPU AVX2 kernels.
- **Prefill Kernel Stability**: Robust MMQ fallback (`STRATA_PREFILL_MMQ=1`) avoiding illegal CUDA memory accesses during large batch/prefill phases.
- **Speculative Verification**: Tuned draft verification (`--spec 3 --spec-min-p 0.5 --suffix-draft 3`) delivering >80% draft acceptance rate and high decoding throughput.
- **Zero-Dependency Native C++ Client**: Fast SSE streaming client with interactive terminal UI, live token rate telemetry, and automated benchmarking.

---

## 🚀 Quick Start

### 1. Launch the Engine & Interactive Chat
The quickest way to interact with Weaver is using `scripts/run_weaver.sh`. It automatically verifies that the backend daemon is running (starting it if needed) and attaches an interactive session:

```bash
./scripts/run_weaver.sh
```

### 2. Single-Prompt Generation
Generate a response directly to stdout with real-time streaming:

```bash
./scripts/run_weaver.sh -p "Write an optimized LRU cache in C++20 with unit tests."
```

### 3. Check Real-Time Telemetry & Health
Inspect engine status, model info, active VRAM/RAM expert allocation, and context limits:

```bash
./scripts/run_weaver.sh -s
```

### 4. Run Speed & Throughput Benchmark
Benchmark tokens-per-second (TPS) on cold and warm queries:

```bash
./scripts/run_weaver.sh -b
```

---

## 🛠️ Backend Management

The inference backend runs in a detached `tmux` session named `weaver_server`.

To start or restart the backend service:
```bash
./scripts/start_backend.sh
```

To view live backend logs or engine diagnostics:
```bash
tmux attach -t weaver_server
# or
tmux capture-pane -pt weaver_server
```

---

## 🏗️ Manual Compilation

To manually build the native `weaver` binary:

```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```
