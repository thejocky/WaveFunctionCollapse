#pragma once

#include <wave.hpp>

#include <utils/dynamic_bitset.hpp>


namespace wfc {

    // Pixel used as unique identifier for pixels, accessing is not 
    #define Pixel uint32_t

    Pixel pixel(int pixelSize, uint8_t red, uint8_t green, uint8_t blue);
    Pixel pixel(int pixelSize, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);


    class InputImage {
        size_t width_, height_;
        Pixel *data_;

        public:

        const expr PixelSize() {return sizeof(Pixel);}

        size_t width() {return width_;}
        size_t height() {return height_;}

        InputImage(size_t width, size_t height);

        void setPixel(size_t x, size_t y, Pixel pixel) {data[y*width_ + x] = pixel;};
        Pixel getPixel(size_t x, size_t y) {return data[y*width_ + x];}
        
        uint32_t *operator[](int i) {return data_[i*width_];}
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







}