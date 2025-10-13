 <div align="center">
<h1> <a href="https://shaurya010.github.io/Collatz-Hash-Cryptographic-Hash-Algorithm-Using-Collatz-Conjecture/" target="_blank">Collatz Hash: Cryptographic Hash Algorithm Using 3x+1 Conjecture</a> </h1>

</div>

## Test Collatz Hash: <a href="https://shaurya010.github.io/Collatz-Hash-Cryptographic-Hash-Algorithm-Using-Collatz-Conjecture/" target="_blank">Click</a> 

## Official Documentation: <a href="https://eprint.iacr.org/2025/1606" target="_blank">Click</a> 

## Overview
Collatz Hash is a novel cryptographic hash algorithm built upon the famous **3x+1 (Collatz) conjecture**, leveraging its complex branching structure for **collision resistance**, **one-wayness**, and **strong diffusion properties**.  

The algorithm outputs fixed-length digests of **256, 384, or 512 bits** and is designed for applications including:
- Password hashing
- HMAC
- Digital signatures
- Integrity verification

---

## Features
- **Message-dependent permutations:** Breaks assumptions required for multicollision attacks like Joux’s attack.
- **Collatz-inspired variable-length mixing loops:** Non-linear iterations governed by the Collatz rule.
- **Resistance to classical attacks:**
  - Differential Cryptanalysis
  - Joux’s Multicollision Attack
  - Fixed-Point Attacks
  - Algebraic Attacks
- **No fixed round constants:** Enhances security over standard designs like MD5 and SHA families.

---

## Algorithm Details

### Hash Sizes
| Variant             | Digest Length | Internal State Size |
|---------------------|---------------|---------------------|
| Collatz Hash-256     | 256 bits       | 512 bits            |
| Collatz Hash-384     | 384 bits       | 512 bits            |
| Collatz Hash-512     | 512 bits       | 512 bits            |

### Compression Function
- **Input:** 1024-bit message block + 512-bit previous state  
- **Output:** Updated 512-bit state  
- **Rounds:** 7 outer rounds × variable-length Collatz iterations  

### Final Output
Digest is generated using message-dependent shuffling and truncation of internal state variables.

---

## Security Highlights

### 1. Avalanche Effect
Differential analysis shows average output bit flip probability close to **0.5** for single-bit input changes, indicating strong diffusion properties.

### 2. Resistance to Attacks
- **Multicollision Attacks:** Message-dependent permutations break the “fixed compression function” assumption.
- **Fixed-Point Attacks:** No global fixed point due to non-linear Collatz dynamics.
- **Algebraic Attacks:** Non-linear and data-dependent round functions prevent low-degree polynomial modeling.

### 3. Length Extension Attack Resistance
Finalization step and truncation prevent standard Merkle–Damgård length extension attacks.

---


```bash
g++ Collatz-Hash.cpp -o collatz_hash
