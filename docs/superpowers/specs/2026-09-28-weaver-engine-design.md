# Weaver Inference Engine - Design Specification

## Overview
Weaver is a custom, high-performance C++/CUDA inference engine designed to achieve 60-70+ TPS on 125B parameter Mixture-of-Expert (MoE) models with a 128K context window. It is specifically optimized to run on consumer hardware with strict VRAM limits (e.g., 8GB RTX 5060) by leveraging massive System RAM (126GB+) and aggressively bypassing memory bandwidth bottlenecks.

## Core Architecture

### 1. Hybrid KV Cache (H2O + Recompute)
To avoid the 32GB KV cache VRAM requirement for a 128K context window:
- **Hot Cache (VRAM):** Implements a Heavy Hitter Oracle (H2O) eviction policy. Only the first 4 tokens (Attention Sinks), the 1,000 most recent tokens, and a dynamic set of high-attention "heavy hitter" tokens (up to 5% of total context) are pinned in the 8GB GPU VRAM.
- **Cold Archive (System RAM):** The remaining 95% of evicted KV tokens are not discarded. Instead, their compressed embeddings are kept in System RAM.
- **On-the-fly Recompute (KV-Direct):** If a query strongly attends to an evicted token, a custom FlashAttention kernel fetches the raw text embedding from System RAM and recomputes the KV states on the fly using spare CUDA cores, trading compute for memory bandwidth.

### 2. Multi-Token Prediction (MTP) Speculative Decoding
To mask the latency of System RAM fetches and guarantee 60-70 TPS:
- **Draft Head:** A lightweight MTP draft head runs entirely in VRAM, predicting 5 to 8 tokens ahead using only the Hot Cache.
- **Parallel Verification:** The 125B core MoE model verifies the 8 drafted tokens in a single batch pass. This drastically reduces the number of System RAM memory reads per token, increasing the TPS ceiling.

### 3. Execution & Memory Management
- **Language:** C++ with raw CUDA kernels. No Python runtime overhead during the generation loop.
- **Unified Memory Allocator:** Implements a custom page-locked (pinned) memory allocator across the GPU and Host (System RAM). This prevents OS-level OOM (Out Of Memory) kills by strictly capping CUDA allocations and gracefully overflowing to mapped host memory.

## Data Flow
1. **Prefill:** The 128K prompt is chunked (e.g., 1024 tokens at a time). As KV states are computed, the Heavy Hitter algorithm evaluates attention scores.
2. **Eviction:** Low-score KV states are evicted to the Cold Archive (System RAM).
3. **Drafting:** The MTP head drafts 8 tokens using the surviving Hot Cache.
4. **Verification:** The drafted tokens are verified. If a cold token is suddenly needed (attention spike), it is streamed back to the GPU and recomputed.

## Success Criteria
- Sustains >60 TPS during autoregressive generation on an RTX 5060.
- Gracefully handles a 128K token prompt without crashing or throwing CUDA `illegal memory access` errors.
- Output quality remains comparable to standard un-evicted inference.
