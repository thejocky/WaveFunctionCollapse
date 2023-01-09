#include <dynamic_bitset.hpp>

#include <algorithm>


    
DynamicBitset::DynamicBitset(size_t n)
    data_(new uint32_t[n/BITS_IN_I32]{0}),
    size_(n), numBlocks_(n/BITS_IN_I32)
{}

// template <T>
// DynamicBitset(T data) 
//     size_(n), numBlocks_(n/BITS_IN_I32)
// {

// }
// DynamicBitset(const DynamicBitset& other);


bool DynamicBitset::bit(size_t n) {
    return data[n/BITS_IN_I32] & (0x8 << n % BITS_IN_I32)
}
uint32_t DynamicBitset::blockN(size_t n) {return data[n/BITS_IN_I32];}


    
    
    
// Bitwise opperations

// applys & operation on each bit of self with other bitset
DynamicBitset& DynamicBitset::and(const DynamicBitset& other) {
    if (other.size() > size_) return other & *self;
    DynamicBitset result(size_);
    for (auto i = 0; i < other.blockSize(); i++) {
        result.setBlock(i, data_[i] & other.block(i));
    }
    return result;
}

// applys | operation on each bit of self with other bitset
DynamicBitset& DynamicBitset::or(const DynamicBitset& other) {
    if (other.size() > size_) return other | *self;
    DynamicBitset result(size_);
    for (auto i = 0; i < other.blockSize(); i++) {
        result.setBlock(i, data_[i] | other.block(i));
    }
    for (auto i = other.blockSize(); i < numBlocks_; i++) {
        result.setBlock(i, data[i]);
    }
    return result;
}

// applys ^ operation on each bit of self with other bitset
DynamicBitset& DynamicBitset::xor(const DynamicBitset& other) {
    if (other.size() > size_) return other ^ *self;
    DynamicBitset result(size_);
    for (auto i = 0; i < other.blockSize(); i++) {
        result.setBlock(i, data_[i] ^ other.block(i));
    }
    for (auto i = other.blockSize(); i < numBlocks_; i++) {
        result.setBlock(i, data[i]);
    }
    return result;
}

// // Shift each bit left by n filling in with 0
// DynamicBitset& shiftLeft(int n) {
//     DynamicBitset result(size_);
//     for (auto i = 0; i < ; i++) {
//         result.setBlock(i, data_[i] ^ other.block(i));
//     }
// }

// // shift each bit right by n filling in with 0
// DynamicBitset& shiftRight(n);
// // Shift Algorithmic Right: Shift each bit right while preserving sign bit
// // 10110010 -> 11011001 | 00110010 -> 00011001 
// DynamicBitset& shiftARight(n);
// Invert each bit of bitset

DynamicBitset& DynamicBitset::not() {
    DynamicBitset result(size_);
    for (auto i = 0; i < numBlock; i++) {
        result.setBlock(i, ~data_[i]);
    }
    return result;
}

