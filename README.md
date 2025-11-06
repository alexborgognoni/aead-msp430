# AEAD Algorithms for MSP430 Microcontrollers

Hand-optimized MSP430 assembly implementations of NIST Lightweight Cryptography competition finalists, developed as part of the University of Luxembourg's contribution to the NIST LWC standardization process.

## Overview

This repository contains optimized implementations of lightweight Authenticated Encryption with Associated Data (AEAD) algorithms for 16-bit MSP430 microcontrollers. The focus is on maximizing performance through hand-written assembly for the performance-critical permutation components.

## Implemented Algorithms

### GIFT-COFB (2 variants)

- **Standard implementation**: Basic GIFT-128 block cipher with COFB mode
- **Fixsliced variant**: Optimized representation for improved MSP430 performance
- 128-bit block size, 128-bit key, 40 rounds
- NIST LWC finalist

### TinyJambu

- Lightweight AEAD optimized for resource-constrained IoT devices
- 128-bit state and key size
- 128-round state update function
- NIST LWC finalist

### Xoodyak

- Sponge-based AEAD built on the Xoodoo permutation
- 384-bit state (12 × 32-bit words)
- Selected as NIST LWC finalist, though ASCON was ultimately chosen as the standard

## Optimizations

The implementations leverage MSP430-specific assembly optimizations:

- **Aggressive register allocation**: Full cipher state held in registers to minimize memory access
- **Bit manipulation acceleration**: Efficient use of rotate-through-carry, byte swap, and bit test instructions
- **Loop unrolling**: Reduces branch overhead for round-based operations
- **Macro-based design**: Prevents code duplication while maintaining performance
- **16-bit pair manipulation**: Efficient handling of 32-bit operations on 16-bit architecture

## Structure

Each algorithm implementation includes:

```
algorithm-msp/
├── main.c                    # Test harness with reference C implementations
├── mspasm/                   # Hand-optimized MSP430 assembly
│   └── *_perm_msp.s43        # Permutation implementations
├── *.h                       # Algorithm specifications and constants
└── *.ewp                     # IAR Embedded Workbench project files
```

## Background

This work was part of the **Sparkle team's** contribution to the NIST Lightweight Cryptography standardization effort at the **University of Luxembourg** (2021-2022). The Sparkle suite (Schwaemm AEAD and Esch hash functions) was among the 10 finalists in the competition, which concluded with ASCON being selected as the standard in 2023.

## Testing

Each implementation includes test functions that:

1. Initialize test vectors with deterministic values
2. Run reference C implementations
3. Execute optimized assembly implementations
4. Verify outputs for correctness

## References

- **NIST Lightweight Cryptography**: https://csrc.nist.gov/projects/lightweight-cryptography
- **Sparkle Suite**: https://sparkle-lwc.github.io/
- **Sparkle Repository**: https://github.com/cryptolu/sparkle
- **MSP430 AEAD Benchmarks**: https://github.com/JohGroLux/AEAD430
- **Publication**: "Lightweight Permutation-Based Cryptography for the Ultra-Low-Power Internet of Things" (Springer, 2023)

## Team

Part of the University of Luxembourg's cryptography research group (DCS and SnT), contributing to the NIST lightweight cryptography standardization process.

## Build

Projects configured for IAR Embedded Workbench IDE. Binary artifacts (`.d43` format) included for MSP430F1611 target.

---

_Developed January 2022 as part of NIST LWC competition benchmarking efforts_
