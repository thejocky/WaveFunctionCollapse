#pragma once

namespace wfc {
    class Tile;
    class Wave;
    enum WaveDirection {UP=0, RIGHT=1, DOWN=2, LEFT=3};
    struct Coords {int x; int y;};
}

#include <vector>
#include <utils/array2D.hpp>
#include <utils/dynamic_bitset.hpp>
#include <cstddef>
#include <bitset>
#include <forward_list>

#include <wave_input_data.hpp>

#include <iostream>

namespace wfc {

    class Tile {
        DynamicBitset states_;  // bitset of possible states of tile
        double weightSum_;      // Sum of weights of possible states
        double entropy_;
        bool collapsed_;
        size_t finalState_; // Once collapsed is the final state

        public:

        Tile (size_t numberOfStates, input::RuleSet &rules);

        ~Tile() {}

        double updateEntropy(input::RuleSet &rules);
        double entropy() {return entropy_;}


        void collapse(input::RuleSet &rules);

        void enforceRule(Coords position, Array2D<Tile*> &waveGrid_,
            input::RuleSet &rules_, DynamicBitset &rule_);

        void propagate(Coords position, Array2D<Tile*> &waveGrid_,
            input::RuleSet &rules_);

        size_t finalState();

        void printTile();

    };

    class Wave {

        Array2D<Tile*> waveGrid_;
        std::forward_list<Tile*> entropySorted_; // Tiles in grid sorted by entropy 
        bool collapsed_;
        
        input::RuleSet *rules_;

        public:

        

        Wave(size_t width, size_t height, size_t states, input::RuleSet *rules);
        
        ~Wave() {}

        

        // Returns tile with the lowest entropy in grid
        Coords lowestEntropy();

        // Callapse tile based on weights
        bool collapseTile(Coords position);

        // Propigate any changes from collapsed tile
        bool propigate();

        // collapse lowest entropy tile until full grid is collapsed or collision occurs
        bool collapse();

        // returns final state as WaveGrid
        input::WaveGrid* saveToWaveGrid();

        void printWave();

    };

}
