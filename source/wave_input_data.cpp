#include <wave_input_data.hpp>

#include <iostream>

namespace wfc::input {


    InputGrid::InputGrid(uint32_t width, uint32_t height) :
        width_(width), height_(height), data_(new TileID[width*height]{NULL})
    {}

    InputGrid::InputGrid(uint32_t width, uint32_t height, TileID* data) :
        width_(width), height_(height), data_(data), numStates_(0)
    {
        for (int x = 0; x < width_; x++) {
            for (int y = 0; y < height_; y++) {
                if (getTile(x, y)+1 > numStates_) numStates_ = getTile(x, y)+1;
            }
        }
    }

    void InputGrid::setTile(size_t x, size_t y, TileID tile) {
        if (tile > numStates_) numStates_ = tile;
        data_[y*width_+x] = tile;
    }

    InputGrid::~InputGrid() {
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




    bool RuleSet::addInput(const InputGrid& grid, ImageLoader& loader) {
        uint32_t state;

        if (grid)

        for (int x = 0; x < grid.width(); x++) {
            for (int y = 0; y < grid.height(); y++) {

                state = grid.getTile(x, y);
                counts_[state]++;
                
                // Set upward as rule
                if (y != 0)
                    rules_[state][Wave::UP].setBit(grid.getTile(x, y-1));
                
                // Set leftward as rule
                if (x != 0)
                    rules_[state][Wave::LEFT].setBit(grid.getTile(x-1, y));
                
                // Set upward as rule
                if (y != grid.heigth()-1) 
                    rules_[state][Wave::UP].setBit(grid.getTile(x, y+1));   
            
                // Set upward as rule
                if (y != grid.width()-1)
                    rules_[state][Wave::UP].setBit(grid.getTile(x+1, y));
            }
        }
        return true;
    }

    bool RuleSet::addImageData(uint8_t* image, uint32_t width, uint32_t height, uint32_t channels, ImageLoader& loader) {
        InputGrid grid = grid(width, height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Tile tile;
                memcpy(&tile, data + (x+y*width)*channels, channels);
                grid.setTile(tile);
            }
        }
        return addInput(grid);
    }


    bool RuleSet::addImage(const char* path, ImageLoader& loader) {
        int width, height, nrChannels;
        unsigned char* data;

        data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (!data) {std::cerr << "ERRROR OPENING IMAGE: " << file << "\n"; return false;}
        auto returnVal = addImageData(data, width, height, channels, loader);
        delete[] data;
        return returnVal;
    }

    DynamicBitset& RuleSet::getRule(int state, Wave::Direction direction) {
        return rules_[state][direction];
    }


}