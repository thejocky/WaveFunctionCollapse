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

        uint8_t* data;
        uint32_t width_, height_, channels_;

        public:

        InputGrid(size_t width, size_t height);

        size_t width() {return width_;}
        size_t height() {return height_;}

        void setTile(size_t x, size_t y, Tile tile);
        Tile getTile(size_t x, size_t y);

        // Tile *operator[](int i) {return data_[i*width_];}
    }



        // void setTile(size_t x, size_t y, Tile tile) override {data[y*width_ + x] = tile;}
        // Tile getTile(size_t x, size_t y) override {return data[y*width_ + x];}

    class ImageLoader {
        std::map<Pixel, Tile> encodingMap_;
        std::map<Tile, Pixel> decodingMap_;
        bool implicitEncoding_;

        public:

        ImageLoader() {}
        ~ImageLoader() {}

        void loadRulesFromImage()

        InputGrid* loadImage(const char* path);

        Tile encodePixel(Pixel pixel) {return encodingMap_[pixel];}
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
        bool addImage(uint8_t* image, ImageLoader& loader);
        bool addImageFromFile(const char* path, ImageLoader& loader);


        bool addRulesFromFile(const char* path);
        bool reset() {rules_.clear();}

        bool loadToFile(const char* path);

        void getWeight(int state) {return weights_[state];}
        void getRule(int state, Wave::Direction direction);
        
    };

    







}