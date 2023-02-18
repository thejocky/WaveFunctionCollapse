#pragma once

#include <cstdlib>
// #include <bitset>
#include <vector>
#include <stdint.h>

class DynamicBitset {

    static const int BITS_IN_I32 = sizeof(uint32_t) * 8;
    
    std::vector<uint32_t> data_; // bits orientated backwards so lowest order bit at index 0
    size_t size_;

    public:
    
    DynamicBitset(size_t n);
    DynamicBitset(size_t n, bool value);
    DynamicBitset(const DynamicBitset &other);

    // ~DynamicBitset();
    // template <T>
    // DynamicBitset(T data);
    // DynamicBitset(long long data);

    protected:

    const std::vector<uint32_t> &data() const {return data_;}

    public:

    void expand(size_t size);

    size_t size() const {return size_;}
    bool bit(size_t n) const;
    void setBit(size_t n, bool value);

    void setAll(bool value);

    size_t blockSize() const {return data_.size();}
    uint32_t block(size_t n) const;
    void setBlock(size_t n, uint32_t block);

    

    bool isTrue() const;
    
    
    // Bitwise opperations (NOT YET USABLE FOR OPERATION WITH DIFFERENT SIZES)

    // applys & operation on each bit of self with other bitset
    static void andOp(const DynamicBitset& src,
        const DynamicBitset& target, DynamicBitset& dest);
    // applys | operation on each bit of self with other bitset
    static void orOp(const DynamicBitset& src,
        const DynamicBitset& target, DynamicBitset& dest);
    // applys ^ operation on each bit of self with other bitset
    static void xorOp(const DynamicBitset& src,
        const DynamicBitset& target, DynamicBitset& dest);
    // // Shift each bit left by n filling in with 0
    // DynamicBitset& shiftLeft(int n);
    // // shift each bit right by n filling in with 0
    // DynamicBitset& shiftRight(n);
    // // Shift Algorithmic Right: Shift each bit right while preserving sign bit
    // // 10110010 -> 11011001 | 00110010 -> 00011001 
    // DynamicBitset& shiftARight(n);
    // Invert each bit of bitset
    static void notOp(const DynamicBitset& src, DynamicBitset& dest);

    DynamicBitset& operator& (const DynamicBitset& other);
    DynamicBitset& operator| (const DynamicBitset& other);
    DynamicBitset& operator^ (const DynamicBitset& other);
    DynamicBitset& operator~ ();

    DynamicBitset& operator&= (const DynamicBitset& other);
    DynamicBitset& operator|= (const DynamicBitset& other);
    DynamicBitset& operator^= (const DynamicBitset& other);

    operator bool() const {return isTrue();};





};