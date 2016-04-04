Random Linear Network Coding
-----------------
This is a tiny Random Linear Network Coding (RLNC) implementation, encode/decode a block a time, while a block (or called generation) is consists of symbols. The block size is the sum of symbols, such (symbol_size * no. symbols).
For now it only supports symbols = 8 (1 byte vector operation), and GF(2) field exclusive-or operation.

Future Work
-----------------
* Implement AVX/SIMD support(or NEON on ARM platform)
* Support flexible number of symbols (only 8-bit vector available now)
* Less memcpy function calls and less memory use
* Object-Oriented API interface
* Other coding schemes (ex. on the fly, systematic)

Reference
-----------------
* [kodo] (https://github.com/steinwurf) 
