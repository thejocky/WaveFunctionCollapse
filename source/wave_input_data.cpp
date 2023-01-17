#include <wave_input_data.hpp>

namespace wfc {


    bool RuleSet::loadImage(const char* path) {
        int width, height, nrChannels;
        unsigned char* data;

        data = stbi_load(file, &width, &height, &nrChannels, 0);
        if (!data) {std::cerr << "ERRROR OPENING TEXTURE: " << file << "\n"; return false;}

        width_ = width;
        height_ = height;

        



    }
    bool RuleSet::loadRulesFromFile(const char* path);

    void getRule(int state, Wave::Direction direction);




}