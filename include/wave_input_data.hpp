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
        size_t width_, height_;
        Tile *data_;

        public:

        InputGrid(size_t width, size_t height);

        const expr TileSize() {return sizeof(Tile);}

        size_t width() {return width_;}
        size_t height() {return height_;}

        void setTile(size_t x, size_t y, Tile tile) {data[y*width_ + x] = tile;};
        Tile getTile(size_t x, size_t y) {return data[y*width_ + x];}
        
        Tile *operator[](int i) {return data_[i*width_];}
    }

    class RuleSet {


        int states_;
        float* wieghts_;
        std::vector<DynamicBitset[4]> rules_;

        public:

        RuleSet() : states_(0) {}
        ~RuleSet() {}

        bool addImage(InputImage image);
        bool addImage(const char* path);
        bool addRulesFromFile(const char* path);
        bool reset() {rules_.clear();}

        bool loadToFile(const char* path);

        void getWeight(int state) {return weights_[state];}
        void getRule(int state, Wave::Direction direction);
        
    };

    class ImageLoader {
        std::map<Pixel, Tile> encodingMap_;
        std::map<Tile, Pixel> decodingMap_;
        bool implicitEncoding_;

        public:

        ImageLoader() {}
        ~ImageLoader() {}

        void setImplicitEncoding(bool value) {implicitEncoding_ = true;}

        InputGrid* loadImage(const char* path);
        

    };







}