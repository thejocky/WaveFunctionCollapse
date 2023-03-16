#pragma once

namespace wfc {
    class Tile;
    class Wave;
    // UP RIGHT DOWN and LEFT are used for indexing ruleset,
    // NONE is only used as default direction 
    enum WaveDirection {UP=0, RIGHT=1, DOWN=2, LEFT=3, NONE};
    struct Coords {int x; int y;};
}

#include <vector>
#include <utils/array2D.hpp>
#include <utils/dynamic_bitset.hpp>
#include <cstddef>
#include <bitset>
#include <forward_list>
#include <stdexcept>

#include <wave_input_data.hpp>

#include <iostream>

namespace wfc {

    class Tile {
        DynamicBitset states_;  // bitset of possible states of tile
        double weightSum_;      // Sum of weights of possible states
        double entropy_;
        bool collapsed_;
        size_t finalState_; // Once collapsed is the final state

        int* propagations_TMP_;

        static constexpr int QUEUE_SIZE = 1024;


        struct PropagationTarget {
            Coords coords; WaveDirection direction;
        };
        template<typename T, int SIZE>
        class PropagationQueue {
            int begin_;
            int end_;
            T data_[SIZE];
            public:
            PropagationQueue() :
                begin_(0), end_(0)
            {}
            inline void push(T value) {
                data_[end_] = value;
                if (++end_ >= SIZE) end_=0;
                if (end_ == begin_) {end_--; throw std::overflow_error("Stack overflow in propagation queue");}
            }
            inline T pop() {
                if (end_ == begin_) throw std::underflow_error("Stack overflow in propagation queue");
                T tmp = data_[begin_];
                if (++begin_ >= SIZE) begin_=0;
                return tmp;
            }
            inline int size() {return (end_ - begin_) % SIZE;}
            inline bool empty() {return begin_ == end_;}
        };

        public:

        Tile (const input::RuleSet &rules, int* propagations_TMP);

        ~Tile() {}

        bool collapsed() {return collapsed_;}

        void reset(const input::RuleSet &rules);

        double updateEntropy(const input::RuleSet &rules);
        double entropy() {return entropy_;}


        void collapse(const input::RuleSet &rules, Coords position);

        static void propagate(Coords position, Array2D<Tile*> &waveGrid_, const input::RuleSet &rules_);

        void enforceRule(Coords position, Array2D<Tile*> &waveGrid,
            const input::RuleSet &rules, DynamicBitset &enforcedRule,
            PropagationQueue<PropagationTarget, QUEUE_SIZE> &queue,
            WaveDirection direction = WaveDirection::NONE);

        void localPropagate(
            Coords position, Array2D<Tile*> &waveGrid, const input::RuleSet &rules,
            PropagationQueue<PropagationTarget, QUEUE_SIZE> &queue, 
            WaveDirection direction = WaveDirection::NONE);

        size_t finalState();

        void printTile();

    };

    class Wave {

        Array2D<Tile*> waveGrid_;
        bool initialized_; // If waveGrid has been populated with Tiles

        std::forward_list<Tile*> entropySorted_; // Tiles in grid sorted by entropy 
        bool collapsed_; // If the wave is in a collapsed state
        
        const input::RuleSet *rules_;
        bool rulesOwnership_; // If Wave class has ownership of rules

        int propagations_TMP_;

        public:

        Wave(size_t width, size_t height);
        ~Wave() {}

        bool collapsed() {return collapsed_;}

        void initialize(const input::RuleSet *rules, bool ownership = false);

        private:

        // Returns tile with the lowest entropy in grid
        Coords lowestEntropy();
        // Callapse tile based on weights
        bool collapseTile(Coords position);
        // // Propigate any changes from collapsed tile
        // bool propigate();

        public:

        void collapseLowestEntropy();

        // collapse lowest entropy tile until full grid is collapsed or collision occurs
        bool collapse();

        bool reset();

        // returns final state as WaveGrid
        input::WaveGrid* saveToWaveGrid() const;

        void printWave();

    };

}
