#include <wave_input_data.hpp>

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <dep/stb/stb_image.h>
#include <dep/stb/stb_image_write.h>

namespace wfc::input {


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
        
        if (tile > numStates_) {numStates_ = tile;
        // std::cout << tile << " - " << numStates_ << " - " << x << ", " << y << "\n";
        }
        data_[y*width_+x] = tile;
    }

    WaveGrid::~WaveGrid() {
        delete[] data_;
    }

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
        if (!pixel) std::cout << "TILE HAS NO CORISPONDING PIXEL TO DECODE TO.";
        return pixel;
    }

    bool ImageLoader::saveAsImage(WaveGrid* grid, const char* filePath) {
        uint8_t* data = reinterpret_cast<uint8_t*>(grid->getInternalData());
        stbi_write_png(filePath, grid->width(), grid->height(), 4, data, 0);

    }



    RuleSet::RuleSet(uint32_t states) :
        states_(states), weights_(states, 0), counts_(states,0), processedStates_(0),
        rules_(4, std::vector(states, DynamicBitset(states)))
    {}

    bool RuleSet::addInput(const WaveGrid& grid, ImageLoader& loader) {
        uint32_t state;
        // std::cout << grid.numStates() << " : " << states_ << "\n";
        if (grid.numStates() > states_) {
            std::cerr << "ERROR INPUT DATA USED STATES ABOVE CAPACITY OF RULESET.\n";
            return false;
        }

        for (int y = 0; y < grid.width(); y++) {
            for (int x = 0; x < grid.height(); x++) {

                state = grid.getTile(x, y)-1;
                counts_[state]++;
                // std::cout << "adding rule - " << "pos:" << x << ',' << y << " - state:" << state << " - ";
                
                // Set upward as rule
                if (y != 0)
                    rules_[WaveDirection::UP][state].setBit(grid.getTile(x, y-1), true);
                
                // std::cout << "Up finished:" << rules_[WaveDirection::DOWN][3].size() << " - ";

                // Set leftward as rule
                if (x != 0)
                    rules_[WaveDirection::LEFT][state].setBit(grid.getTile(x-1, y), true);
                
                // std::cout << "Left finished - ";

                // Set rightward as rule
                rules_[WaveDirection::UP][state];
                if (y != grid.height()-1) 
                    rules_[WaveDirection::DOWN][state].setBit(grid.getTile(x, y+1), true);   
                    // std::cout << "pointer: " << rules_[WaveDirection::UP][state].size() << "\n";   
                // std::cout << "Right finished - ";
            
                // Set downward as rule
                if (x != grid.width()-1)
                    rules_[WaveDirection::RIGHT][state].setBit(grid.getTile(x+1, y), true);
                // std::cout << "finished\n";
            }
        }
        return true;
    }

    bool RuleSet::addImageData(uint8_t* image, uint32_t width, uint32_t height, uint32_t channels, ImageLoader& loader) {
        WaveGrid grid(width, height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Pixel pixel;
                memcpy(&pixel, image + (x+y*width)*channels, channels);
                grid.setTile(x, y, loader.encodePixel(pixel));
            }
        }
        return addInput(grid, loader);
    }

    bool RuleSet::addImage(const char* path, ImageLoader& loader) {
        int width, height, nrChannels;
        unsigned char* data;

        data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (!data) {std::cerr << "ERRROR OPENING IMAGE: " << path << "\n"; return false;}
        auto returnVal = addImageData(data, width, height, nrChannels, loader);
        delete[] data;
        return returnVal;
    }

    void RuleSet::reset() {
        rules_[0].clear();
        rules_[1].clear();
        rules_[2].clear();
        rules_[3].clear();
    }


    DynamicBitset& RuleSet::getRule(int state, WaveDirection direction) {
        return rules_[direction][state];
    }

}