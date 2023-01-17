#pragma once

#include <wave.hpp>

#include <utils/dynamic_bitset.hpp>


namespace wfc {


    class InputImage {
        size_t width_, height_, pixelSize_;
        uint8_t *data_;
        
        protected:

        size_t width() {return width_;}
        size_t height() {return height_;}
        size_t pixelSize() {return pixelSize_;}

        public:

        class slice {
            InputImage* source_;
            uint8_t* data_;
            public:
            Slice(uint8_t* row, InputImage* source) : data_(row), source_(source) {}
            ~Slice() {}
            uint8_t *operator[](int i) {return data[i*source_->pixelSize()];}
        }
        
        slice *operator[](int i) {return }
    }

    class RuleSet {


        int states_;
        std::vector<DynamicBitset[4]> rules_;

        public:

        RuleSet() : states_(0) {}
        ~RuleSet() {}

        bool loadImage(const char* path);
        bool loadRulesFromFile(const char* path);

        void getRule(int state, Wave::Direction direction);
        
    };







}