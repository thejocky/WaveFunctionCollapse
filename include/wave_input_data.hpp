#pragma once

#include <wave.hpp>

#include <utils/dynamic_bitset.hpp>


namespace wfc::input {

    

    // Tile used as unique identifier for different possible grid values 
    #define Tile uint32_t

    #define Pixel uint32_t
    Pixel pixel(int TileSize, uint8_t red, uint8_t green, uint8_t blue);
    Pixel pixel(int TileSize, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);


    class InputGrid {
        uint32_t width_, height_;
        Tile* data_;

        // void expandStates(uint32_t num);

        public:

        InputGrid(uint32_t width, uint32_t height);
        InputGrid(uint32_t width, uint32_t height, uint8_t* data);

        size_t width() {return width_;}
        size_t height() {return height_;}

        void setTile(size_t x, size_t y, Tile tile) {returndata_[y*width_+x] = tile;}
        Tile getTile(size_t x, size_t y) {return data_[y*width_+x];}

        Tile *operator[](int y) {return data_[i*width_];}
    }



        // void setTile(size_t x, size_t y, Tile tile) override {data[y*width_ + x] = tile;}
        // Tile getTile(size_t x, size_t y) override {return data[y*width_ + x];}

    class ImageLoader {
        int32_t usedTiles;
        std::map<Pixel, Tile> encodingMap_;
        std::map<Tile, Pixel> decodingMap_;

        public:

        ImageLoader() {}
        ~ImageLoader() {}

        bool addEncoding(Pixel pixel, Tile tile);
        bool addDecoding(Tile tile, Pixel pixel);

        Tile encodePixel(Pixel pixel);
        Pixel decodeTile(Tile tile) {return decodingMap_[tile];}
        

    };



    class RuleSet {


        int states_;                   // Number of states currently in ruleset
        std::vector<float> wieghts_;   // Weights of each state
        std::vector<uint32_t> counts_; // Number of each state encountered in input
        size_t processedStates_;       // Total number of states processed by ruleset 
        std::vector<DynamicBitset[4]> rules_;

        public:

        RuleSet() : states_(0) {}
        ~RuleSet() {}

        bool addInput(InputGrid grid, ImageLoader& loader);
        bool addImageData(uint8_t* image, ImageLoader& loader);
        bool addImage(const char* path, ImageLoader& loader);


        bool addRulesFromFile(const char* path);
        bool reset() {rules_.clear();}

        bool loadToFile(const char* path);

        void getWeight(int state) {return weights_[state];}
        DynamicBitset& getRule(int state, Wave::Direction direction);
        
    };

    







}