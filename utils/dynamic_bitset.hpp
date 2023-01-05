#pragma once

#include <cstdlib>
#include <bitset>

class DynamicBitset {

    static const BITS_IN_I32 = sizeof(i32_t);
    
    uint32_t* data_; // bits orientated backwards so lowest order bit at index 0
    size_t size_;
    size_t numBlocks_;

    public:
    
    DynamicBitset(size_t n);
    template <T>
    DynamicBitset(T data);
    // DynamicBitset(long long data);
    // DynamicBitset(const DynamicBitset& other);

    size_t size() {return size_;}
    bool bit(size_t n)
    bool setBit(size_t n, bool value);
    

    protected:

    size_t blockSize() {return numBlocks_;}
    uint32_t block(size_t n);
    uint32_t setBlock(size_t n, uint32_t block);

    public:
    
    
    
    // Bitwise opperations

    // applys & operation on each bit of self with other bitset
    DynamicBitset& and(const DynamicBitset& other);
    // applys | operation on each bit of self with other bitset
    DynamicBitset& or(const DynamicBitset& other);
    // applys ^ operation on each bit of self with other bitset
    DynamicBitset& xor(const DynamicBitset& other);
    // // Shift each bit left by n filling in with 0
    // DynamicBitset& shiftLeft(int n);
    // // shift each bit right by n filling in with 0
    // DynamicBitset& shiftRight(n);
    // // Shift Algorithmic Right: Shift each bit right while preserving sign bit
    // // 10110010 -> 11011001 | 00110010 -> 00011001 
    // DynamicBitset& shiftARight(n);
    // Invert each bit of bitset
    DynamicBitset& not();



};