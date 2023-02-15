#include <wfc.hpp>

int main () {
    uint8_t* finalImages[10] = {nullptr};

    wfc::input::ImageLoader imageLoader;
    wfc::input::RuleSetBuilder ruleBuilder;

    // Convert image into the ruleset
    ruleBuilder.addImage("../test_files/input_2.png", imageLoader);

    wfc::Wave wave(100, 100);
    wave.initialize(ruleBuilder.referenceRuleSet());

    // input::WaveGrid* 
    for (int i = 0; i < 10; i++) {
        wave.collapse();
        finalImages[i] = imageLoader.convertToImage(wave);
        wave.reset();
    }

    


}