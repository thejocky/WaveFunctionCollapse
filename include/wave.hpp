#pragma once

#include <vector>
#include <utils/array2D.hpp>
#include <utils/dynamic_bitset.hpp>
#include <cstddef>
#include <bitset>
#include <forward_list>


class Tile {
    DynamicBitset states_; // bitset of possible states of tile
    const double* weights_;    // Weights for each states
    double weightSum_;        // Sum of weights of possible states
    double entropy_;
    bool collapsed_;
    size_t finalState_; // Once collapsed is the final state




    Tile (const double* weights, size_t numberOfStates);

    ~Tile() {}

    double updateEntropy();
    double entropy() {return entropy_;}


    std::size_t collapse();

    std::size_t type();

    void setInternalStates_DEBUG(std::bitset<numStates> states) {states_ = states;}

};

class Wave {
    
    Array2D<Tile*> waveGrid_;
    std::forward_list<Tile*> entropySorted_; // Tiles in grid sorted by entropy 
    const double* weights_;

    public:

    // Simple struct used to pass coords between methods
    struct Coords {std::size_t x; std::size_t y;};

    Wave(std::size_t width, std::size_t height, std::size_t states, const double* weights);
    
    ~Wave() {}

    

    // Returns tile with the lowest entropy in grid
    Coords lowestEntropy();

    // Callapse tile based on weights
    bool collapseTile(Coords position);

    // Propigate any changes from collapsed tile
    bool propigate();

    // collapse lowest entropy tile until full grid is collapsed or collision occurs
    bool collapse();

};
