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

    imageLoader.saveAsImage(finalImages[0], 100, 100, "../test_files/output1.png");
    imageLoader.saveAsImage(finalImages[1], 100, 100, "../test_files/output2.png");
    imageLoader.saveAsImage(finalImages[2], 100, 100, "../test_files/output3.png");
    imageLoader.saveAsImage(finalImages[3], 100, 100, "../test_files/output4.png");
    imageLoader.saveAsImage(finalImages[4], 100, 100, "../test_files/output5.png");
    imageLoader.saveAsImage(finalImages[5], 100, 100, "../test_files/output6.png");
    imageLoader.saveAsImage(finalImages[6], 100, 100, "../test_files/output7.png");
    imageLoader.saveAsImage(finalImages[7], 100, 100, "../test_files/output8.png");
    imageLoader.saveAsImage(finalImages[8], 100, 100, "../test_files/output9.png");
    imageLoader.saveAsImage(finalImages[9], 100, 100, "../test_files/output10.png");

    


}