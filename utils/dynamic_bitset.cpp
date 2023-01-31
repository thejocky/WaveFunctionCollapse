#include <utils/dynamic_bitset.hpp>

#include <algorithm>


    
DynamicBitset::DynamicBitset(size_t n) :
    data_(new uint32_t[n/BITS_IN_I32]{0}),
    size_(n), numBlocks_(n/BITS_IN_I32)
{}


bool DynamicBitset::bit(size_t n) const {
    return data_[n/BITS_IN_I32] & (0x1 << n % BITS_IN_I32);
}
void DynamicBitset::setBit(size_t n, bool value) {
    data_[n/BITS_IN_I32] |= (0x1 << n % BITS_IN_I32);
}


uint32_t DynamicBitset::block(size_t n) const {
    if (n >= numBlocks_) return 0;
    return data_[n/BITS_IN_I32];
}
uint32_t DynamicBitset::setBlock(size_t n, uint32_t block) {
    if (n >= numBlocks_) return 0;
    uint32_t tmp = data_[n/BITS_IN_I32];
    data_[n/BITS_IN_I32] = block;
    return tmp;
}


bool DynamicBitset::isTrue() const {
    for (auto i = 0; i < numBlocks_; i++) {
        if (data_[i]) return true;
    }
    return false;
}
    
    
// Bitwise opperations

// applys & operation on each bit of self with other bitset
void DynamicBitset::andOp(const DynamicBitset& src,
        const DynamicBitset& target, DynamicBitset& dest) {
    for (auto i = 0; i < dest.blockSize(); i++) {
        dest.setBlock(i, src.block(i) & target.block(i));
    }
}

// applys | operation on each bit of self with other bitset
void DynamicBitset::orOp(const DynamicBitset& src,
        const DynamicBitset& target, DynamicBitset& dest) {
    for (auto i = 0; i < dest.blockSize(); i++) {
        dest.setBlock(i, src.block(i) | target.block(i));
    }
}

// applys ^ operation on each bit of self with other bitset
void DynamicBitset::xorOp(const DynamicBitset& src,
        const DynamicBitset& target, DynamicBitset& dest) {
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

void DynamicBitset::notOp(const DynamicBitset& src, DynamicBitset& dest) {
    for (auto i = 0; i < dest.blockSize(); i++) {
        dest.setBlock(i, ~src.block(i));
    }
}


DynamicBitset& DynamicBitset::operator& (const DynamicBitset& other) {
    DynamicBitset returnVal(size_);
    andOp(*this, other, returnVal);
    return returnVal;
}
DynamicBitset& DynamicBitset::operator| (const DynamicBitset& other) {
    DynamicBitset returnVal(size_);
    orOp(*this, other, returnVal);
    return returnVal;
}
DynamicBitset& DynamicBitset::operator^ (const DynamicBitset& other) {
    DynamicBitset returnVal(size_);
    xorOp(*this, other, returnVal);
    return returnVal;
}
DynamicBitset& DynamicBitset::operator~ () {
    DynamicBitset returnVal(size_);
    notOp(*this, returnVal);
    return returnVal;
}


DynamicBitset& DynamicBitset::operator&= (const DynamicBitset& other) {
    DynamicBitset::andOp(*this, other, *this);
    return *this;
}
DynamicBitset& DynamicBitset::operator|= (const DynamicBitset& other) {
    DynamicBitset::orOp(*this, other, *this);
    return *this;
}
DynamicBitset& DynamicBitset::operator^= (const DynamicBitset& other) {
    DynamicBitset::xorOp(*this, other, *this);
    return *this;
}

