#include <wave_input_data.hpp>

namespace wfc::input {


    bool RuleSet::addImage(InputImage image) {
        
    }


    bool RuleSet::addInput(InputGrid grid&, ImageLoader& loader) {
        uint32_t state;

        for (int x = 0; x < grid.width(); x++) {
            for (int y = 0; y < grid.height(); y++) {

                state = grid.getTile(x, y);
                if (state > states_) expandStates(state);
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

        data = stbi_load(file, &width, &height, &nrChannels, 0);
        if (!data) {std::cerr << "ERRROR OPENING IMAGE: " << file << "\n"; return false;}

        return addImageData(data, width, height, channels, loader)
    }


    // InputGrid* loadImage(const char* path) {
    //     

    //     grid = new InputGrid(width, height);

    //     for (int x = 0; x < width; x++) {
    //         for (int y = 0; y < height; y++) {

    //         }
    //     }
    // }



}