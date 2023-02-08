#pragma once

namespace wfc::input {
    class WaveGrid;
    class ImageLoader;
    class RuleSet;
}

#include <wave.hpp>
#include <utils/dynamic_bitset.hpp>

#include <map>

#include <iostream>

namespace wfc::input {

    

    // Tile used as unique identifier for different possible grid values 
    #define TileID uint32_t

    #define Pixel uint32_t
    Pixel pixel(uint8_t red, uint8_t green, uint8_t blue);
    Pixel pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);


    class WaveGrid {
        TileID width_, height_;
        TileID* data_;
        uint32_t numStates_;

        // void expandStates(uint32_t num);

        protected:
        TileID* getInternalData() {return data_;}

        public:

        WaveGrid(uint32_t width, uint32_t height);
        WaveGrid(uint32_t width, uint32_t height, TileID* data);
        ~WaveGrid();

        uint32_t numStates() const {return numStates_;}

        size_t width() const {return width_;}
        size_t height() const {return height_;}

        void setTile(size_t x, size_t y, TileID tile);
        TileID getTile(size_t x, size_t y) const {return data_[y*width_+x];};

        TileID *operator[](int y) {return data_ + y*width_;}

        friend class ImageLoader;
    };


    class ImageLoader {
        int32_t usedTiles;
        std::map<Pixel, TileID> encodingMap_;
        std::map<TileID, Pixel> decodingMap_;

        public:

        ImageLoader();
        ~ImageLoader() {}

        bool addEncoding(Pixel pixel, TileID tile);
        bool addDecoding(TileID tile, Pixel pixel);

        TileID encodePixel(Pixel pixel);
        Pixel decodeTile(TileID tile);

        uint8_t *convertToImage(const WaveGrid *grid);
        uint8_t *convertToImage(const Wave &wave);
        bool saveAsImage(WaveGrid* grid, const char* filePath);

    };

    class RuleSet {
        // Number of states currently in ruleset
        int states_;
        // Weights of each state, ratio of appearance of state to provided input
        std::vector<float> weights_;  
        // Bitset of rules defining possible adjasent tiles
        std::vector<std::vector<DynamicBitset>> rules_;

        public:

        void reset();

        int numStates() const {return states_;}
        float getWeight(int state) const {return weights_[state];}
        DynamicBitset& getRule(int state, WaveDirection direction) const;
    };

    class RuleSetBuilder {

        // Current set of rules held by builder
        RuleSet rules_;
        // Number of each state encountered in input used in getting weights
        std::vector<uint32_t> counts_;
        // Total number of tiles processed by ruleset used in getting weights
        size_t processedTiles_;
        

        public:

        RuleSetBuilder();
        ~RuleSetBuilder() {}

        private:
        void updateWeights();

        public:

        const RuleSet* referenceRuleSet() {return &rules_;}
        RuleSet* createRuleSet() {return new RuleSet(rules_);}

        int getStates() {return rules_.numStates();}
        float getWeight(int state) {return rules_.getWeight(state);}
        DynamicBitset& getRule(int state, WaveDirection direction)
            {return rules_.getRule(state, direction);}
        

        bool addInput(const WaveGrid& grid);
        bool addImageData(uint8_t* image, uint32_t width,
            uint32_t height, uint32_t channels, ImageLoader& loader);
        bool addImage(const char* path, ImageLoader& loader);

        // bool addRulesFromFile(const char* path);
        void reset();

        // bool loadToFile(const char* path);
        
    };

    







}