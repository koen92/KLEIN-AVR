This repository contains our implementation of the KLEIN-64 cipher \[1\] (optimised for high speed) on the AVR ATTiny45 microcontroller. The ATTiny45 is an 8-bit processor that has 32 registers, 512 bytes SRAM and 4096 bytes Flash memory. As it uses 16-bit addresses, addressing memory requires two 8-bit registers. It is worth mentioning that only register 30 and 31 can be used to load from Flash memory.

### The KLEIN cipher

The KLEIN cipher is a lightweight block cipher that shows many similarities with the Rijndael (AES) cipher. KLEIN consists of roughly the same five operations: AddRoundKey, SubNibbles, RotateNibbles, MixNibbles and KeySchedule. The main difference with AES is the fact that KLEIN operates on nibbles rather than bytes.

   **AddRoundKey** This operation consists of xoring the round key into the state.<br>
   **SubNibbles** In this step, the state is passed through the sbox, one nibble at a time.<br>
   **RotateNibbles** This operation rotates the state to the left by four nibbles.<br>
   **MixNibbles** This step consists of a series of permutations that can be represented as two matrix multiplications over a finite field. It operates on pairwise concatenations of nibbles as elements of an input vector, effectively mimicking the Rijndael MixColumn operation on bytes.<br>
   **KeySchedule** This operation computes the next round key by performing several shifts, sbox lookups and xor operations.<br>

### Our implementation

To run our implementation of the KLEIN cipher with a different input, change line 6 to 13 and line 28 to 35 according to the key and plaintext values. The initial key and plaintext are loaded into registers using `ldi` instructions, and the resulting ciphertext ends up in register 16 to 23. During the execution of the program we keep the round key in register 0 to 7, while the state is alternately in register 8 to 15 and register 16 to 23 depending on the parity of the round.

### Graphical overview of KLEIN

<p align="center">
    <img src="https://github.com/koen92/KLEIN-AVR/raw/master/docs/diagram.png" alt="KLEIN-64" />
</p>

### References

[1] Gong, Z., Nikova, S., & Law, Y. W. (2012). KLEIN: a new family of lightweight block ciphers. In _RFID. Security and Privacy_ (pp. 1-18). Springer Berlin Heidelberg.
