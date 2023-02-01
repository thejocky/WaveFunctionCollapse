#pragma once

namespace wfc::input {
    class InputGrid;
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
    Pixel pixel(int TileSize, uint8_t red, uint8_t green, uint8_t blue);
    Pixel pixel(int TileSize, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);


    class InputGrid {
        TileID width_, height_;
        TileID* data_;
        uint32_t numStates_;

        // void expandStates(uint32_t num);

        public:

        InputGrid(uint32_t width, uint32_t height);
        InputGrid(uint32_t width, uint32_t height, TileID* data);
        ~InputGrid();

        uint32_t numStates() const {return numStates_;}

        size_t width() const {return width_;}
        size_t height() const {return height_;}

        void setTile(size_t x, size_t y, TileID tile);
        TileID getTile(size_t x, size_t y) const {
            std::cout << x << ", " << y << "\n";
            return data_[y*width_+x];}

        TileID *operator[](int y) {return data_ + y*width_;}
    };



        // void setTile(size_t x, size_t y, Tile tile) override {data[y*width_ + x] = tile;}
        // Tile getTile(size_t x, size_t y) override {return data[y*width_ + x];}

    class ImageLoader {
        int32_t usedTiles;
        std::map<Pixel, TileID> encodingMap_;
        std::map<TileID, Pixel> decodingMap_;

        public:

        ImageLoader() {}
        ~ImageLoader() {}

        bool addEncoding(Pixel pixel, TileID tile);
        bool addDecoding(TileID tile, Pixel pixel);

        TileID encodePixel(Pixel pixel);
        Pixel decodeTile(TileID tile);
        

    };



    class RuleSet {


        int states_;                   // Number of states currently in ruleset
        std::vector<float> weights_;   // Weights of each state
        std::vector<uint32_t> counts_; // Number of each state encountered in input
        size_t processedStates_;       // Total number of states processed by ruleset 
        std::vector<DynamicBitset> rules_[4];

        public:

        RuleSet(uint32_t states);
        ~RuleSet() {}

        private:
        void updateWeights();

        public:

        int getStates() {return states_;}
        

        bool addInput(const InputGrid& grid, ImageLoader& loader);
        bool addImageData(uint8_t* image, uint32_t width,
            uint32_t height, uint32_t channels, ImageLoader& loader);
        bool addImage(const char* path, ImageLoader& loader);

        // bool addRulesFromFile(const char* path);
        void reset();

        // bool loadToFile(const char* path);

        float getWeight(int state) {return weights_[state];}
        DynamicBitset& getRule(int state, WaveDirection direction);
        
    };

    







}