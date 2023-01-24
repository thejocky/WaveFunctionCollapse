#include <wave_input_data.hpp>

namespace wfc::input {


    bool RuleSet::addImage(InputImage image) {
        
    }


    bool RuleSet::addInput(InputGrid grid, ImageLoader& loader) {
        uint32_t state;
        for (int x = 0; x < grid.width(); x++) {
            state = grid.getTile(x, 0);
            if (state >) 
        }

        



    }
    // bool RuleSet::loadRulesFromFile(const char* path);

    // void getRule(int state, Wave::Direction direction);



    ImageLoader::loadRulesFromImage(unsigned char* image) {


    }


    // InputGrid* loadImage(const char* path) {
    //     int width, height, nrChannels;
    //     unsigned char* data;
    //     InputGrid* grid;

    //     data = stbi_load(file, &width, &height, &nrChannels, 0);
    //     if (!data) {std::cerr << "ERRROR OPENING TEXTURE: " << file << "\n"; return false;}

    //     width_ = width;
    //     height_ = height;

    //     grid = new InputGrid(width, height);

    //     for (int x = 0; x < width; x++) {
    //         for (int y = 0; y < height; y++) {

    //         }
    //     }
    // }



}