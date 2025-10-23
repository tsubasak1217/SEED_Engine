#pragma once
#include <vector>
#include <initializer_list>

/// <summary>
/// 32ビットなどの数の収まりきらないビットを管理する構造体
/// </summary>
struct BitSet{
public:
    BitSet() = default;
    BitSet(uint32_t numBit) : bits(numBit, false){}
    BitSet(uint32_t numBit, uint32_t index, bool value = true) : bits(numBit, false){bits[index] = value;}
    BitSet(std::initializer_list<bool> list) : bits(list){}

public:
    bool Any()const{
        for(const auto& bit : bits){
            if(bit){
                return true;
            }
        }
        return false;
    }

public:
    std::vector<bool> bits;
};

constexpr BitSet operator&(const BitSet& lhs, const BitSet& rhs){
    BitSet result;
    for(uint32_t i = 0; i < lhs.bits.size(); ++i){
        result.bits[i] = lhs.bits[i] && rhs.bits[i];
    }
    return result;
}

constexpr BitSet operator|(const BitSet& lhs, const BitSet& rhs){
    BitSet result;
    for(uint32_t i = 0; i < lhs.bits.size(); ++i){
        result.bits[i] = lhs.bits[i] || rhs.bits[i];
    }
    return result;
}