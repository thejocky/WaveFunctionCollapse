#include <wave_input_data.hpp>

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <dep/stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <dep/stb/stb_image_write.h>

namespace wfc::input {

    Pixel pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
        return ((uint32_t)alpha << 0x18) | ((uint32_t)blue << 0x10) |
                ((uint32_t)green << 0x8) | red;
    }

    Pixel pixel(uint8_t red, uint8_t green, uint8_t blue) {
        pixel(red, blue, green, 0xFF);
    }


    // WaveGrid Class Definition ----------------------------------------------

    WaveGrid::WaveGrid(uint32_t width, uint32_t height) :
        width_(width), height_(height), data_(new TileID[width*height]{NULL}),
        numStates_(0)
    {}

    WaveGrid::WaveGrid(uint32_t width, uint32_t height, TileID* data) :
        width_(width), height_(height), data_(data), numStates_(0)
    {
        for (int x = 0; x < width_; x++) {
            for (int y = 0; y < height_; y++) {
                if (getTile(x, y)+1 > numStates_) numStates_ = getTile(x, y)+1;
            }
        }
    }

    void WaveGrid::setTile(size_t x, size_t y, TileID tile) {
        
        if (tile > numStates_) numStates_ = tile;
        data_[y*width_+x] = tile;
    }

    WaveGrid::~WaveGrid() {
        delete[] data_;
    }


    // ImageLoader Class Definition -------------------------------------------

    ImageLoader::ImageLoader() :
        usedTiles(0)
    {}

    bool ImageLoader::addEncoding(Pixel pixel, TileID tile) {
        encodingMap_[pixel] = tile;
    }
    bool ImageLoader::addDecoding(TileID tile, Pixel pixel) {
        decodingMap_[tile] = pixel;
    }

    TileID ImageLoader::encodePixel(Pixel pixel) {
        TileID tile = encodingMap_[pixel];
        if (tile) return tile;
        usedTiles++;
        addEncoding(pixel, usedTiles);
        addDecoding(usedTiles, pixel);
        return usedTiles;
    }
    Pixel ImageLoader::decodeTile(TileID tile) {
        Pixel pixel = decodingMap_[tile];
        if (!pixel) std::cout << "TILE HAS NO CORISPONDING PIXEL TO DECODE TO.\n";
        return pixel;
    }

    uint8_t *ImageLoader::convertToImage(const WaveGrid *grid) {
        uint8_t* data = new uint8_t[grid->width()*grid->height() * 4];
        memcpy(data, grid->getInternalData(), grid->width()*grid->height() * 4);
        return data;
    }
    uint8_t *ImageLoader::convertToImage(const Wave &wave) {
        WaveGrid* grid = wave.saveToWaveGrid();
        uint8_t* data = convertToImage(grid);
        delete grid;
        return data;
    }

    bool ImageLoader::saveAsImage(const uint8_t *data, size_t width,
                        size_t height, const char* filePath) {
        stbi_write_png(filePath, width, height, 4, data, 0);
    }
    bool ImageLoader::saveAsImage(const WaveGrid* grid, const char* filePath) {
        uint8_t* data = convertToImage(grid);
        saveAsImage(data, grid->width(), grid->height(), filePath);
        delete[] data;
    }
    bool ImageLoader::saveAsImage(const Wave &wave, const char* filePath) {
        WaveGrid* grid = wave.saveToWaveGrid();
        bool returnVal = saveAsImage(grid, filePath);
        delete grid;
        return returnVal;
    }


    // RuleSet Class Definition -----------------------------------------------

    RuleSet::RuleSet() : 
        states_(0), rules_(4)
    {}

    void RuleSet::reset() {
        states_ = 0;
        weights_.clear();
        rules_.clear();
    }

    void RuleSet::expandRuleSet(int size) {
        int states_ = size;
        weights_.resize(size);
        for (auto &directionRules : rules_) {
            for (auto &rule : directionRules)
                rule.expand(size);
            directionRules.resize(size, size);
        }
    }

    void RuleSet::setWeight(int state, float value) {
        weights_[state] = value;
    }

    void RuleSet::setRule(int state, WaveDirection direction, int targetState) {
        rules_[direction][state].setBit(targetState, true);
    }

    const DynamicBitset& RuleSet::getRule(int state, WaveDirection direction) const {
        return rules_[direction][state];
    }


    // RuleSetBuilder Class Definition ----------------------------------------

    RuleSetBuilder::RuleSetBuilder() :
        processedTiles_(0)
    {}

    void RuleSetBuilder::updateWeights() {
        std::cout << "updating weights: " << rules_.numStates() << "\n";
        for (int i = 0; i < rules_.numStates(); i++) {
            std::cout << "setting weight " << (((float)counts_[i]) / processedTiles_) << "\n";
            rules_.setWeight(((float)counts_[i]) / processedTiles_, i);
        }
    }

    bool RuleSetBuilder::addInput(const WaveGrid& grid) {
        uint32_t state;
        std::cout << "adding input: " << grid.numStates() << " " << rules_.numStates() << "\n";
        if (grid.numStates() > rules_.numStates()) {
            std::cout << "expanding ruleset\n";
            rules_.expandRuleSet(grid.numStates());
            counts_.resize(grid.numStates());
            // std::cerr << "ERROR INPUT DATA USED STATES ABOVE CAPACITY OF RULESET.\n";
            // return false;
        }

        for (int y = 0; y < grid.width(); y++) {
            for (int x = 0; x < grid.height(); x++) {

                state = grid.getTile(x, y)-1;
                processedTiles_++;
                counts_[state]++;
                
                // Set upward as rule
                if (y != 0)
                    rules_.setRule(state, WaveDirection::UP, grid.getTile(x, y-1)-1);
                
                // Set leftward as rule
                if (x != 0)
                    rules_.setRule(state, WaveDirection::LEFT, grid.getTile(x-1, y)-1);
                // Set downward as rule
                if (y != grid.height()-1) 
                    rules_.setRule(state, WaveDirection::DOWN, grid.getTile(x, y+1)-1);  
                // Set rightward as rule
                if (x != grid.width()-1)
                    rules_.setRule(state, WaveDirection::RIGHT, grid.getTile(x+1, y)-1);
            }
        }
        updateWeights();
        return true;
    }

    bool RuleSetBuilder::addImageData(uint8_t* image, uint32_t width, uint32_t height, uint32_t channels, ImageLoader& loader) {
        WaveGrid grid(width, height);
        Pixel setAlpha = 0; // Set alpha or-ed with each pixel
        if (channels == 3) setAlpha = pixel(0, 0, 0, 0xFF);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // std::cout << "loading pixel\n";
                Pixel pixel;
                memcpy(&pixel, image + (x+y*width)*channels, channels);
                pixel |= setAlpha;
                grid.setTile(x, y, loader.encodePixel(pixel));
            }
        }
        return addInput(grid);
    }

    bool RuleSetBuilder::addImage(const char* path, ImageLoader& loader) {
        int width, height, nrChannels;
        unsigned char* data;
        data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (!data) {std::cerr << "ERRROR OPENING IMAGE: " << path << "\n"; return false;}
        auto returnVal = addImageData(data, width, height, nrChannels, loader);
        delete[] data;
        return returnVal;
    }

    void RuleSetBuilder::reset() {
        rules_.reset();
        counts_.clear();
        processedTiles_ = 0;
    }

}