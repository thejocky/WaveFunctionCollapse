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
uint32_t DynamicBitset::blockN(size_t n) {
    if (n >= numBlocks) return 0;
    return data[n/BITS_IN_I32];
}


    
    
    
// Bitwise opperations

// applys & operation on each bit of self with other bitset
void DynamicBitset::and(const DynamicBitset& src,
        const DynamicBitset& target, const DynamicBitset& dest) {
    for (auto i = 0; i < dest.blockSize(); i++) {
        dest.setBlock(i, src.block(i) & target.block(i));
    }
}

// applys | operation on each bit of self with other bitset
void DynamicBitset::or(const DynamicBitset& src,
        const DynamicBitset& target, const DynamicBitset& dest) {
    for (auto i = 0; i < dest.blockSize(); i++) {
        dest.setBlock(i, src.block(i) | target.block(i));
    }
}

// applys ^ operation on each bit of self with other bitset
DynamicBitset& DynamicBitset::xor(const DynamicBitset& src,
        const DynamicBitset& target, const DynamicBitset& dest) {
    for (auto i = 0; i < dest.blockSize(); i++) {
        dest.setBlock(i, src.block(i) ^ target.block(i));
    }
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

void DynamicBitset::not(const DynamicBitset& src, const DynamicBitset& dest); {
    for (auto i = 0; i < dest.blockSize(); i++) {
        dest.setBlock(i, ~src.block(i));
    }
}


DynamicBitset& DynamicBitset::operator& (const DynamicBitset& other) {
    DynamicBitset(size_) returnVal;
    and(*this, other, returnVal);
    return returnVal;
}
DynamicBitset& DynamicBitset::operator| (const DynamicBitset& other) {
    DynamicBitset(size_) returnVal;
    or(*this, other, returnVal);
    return returnVal;
}
DynamicBitset& DynamicBitset::operator^ (const DynamicBitset& other) {
    DynamicBitset(size_) returnVal;
    xor(*this, other, returnVal);
    return returnVal;
}
DynamicBitset& DynamicBitset::operator~ () {
    DynamicBitset(size_) returnVal;
    not(*this, returnVal);
    return returnVal;
}


DynamicBitset& DynamicBitset::operator&= (const DynamicBitset& other) {
    and(*this, other, *this);
    return *this;
}
DynamicBitset& DynamicBitset::operator|= (const DynamicBitset& other) {
    or(*this, other, *this);
    return *this;
}
DynamicBitset& DynamicBitset::operator^= (const DynamicBitset& other) {
    xor(*this, other, *this);
    return *this;
}
