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

    const char *outputFiles[10] = {
        "../test_files/output1.png",
        "../test_files/output2.png",
        "../test_files/output3.png",
        "../test_files/output4.png",
        "../test_files/output5.png",
        "../test_files/output6.png",
        "../test_files/output7.png",
        "../test_files/output8.png",
        "../test_files/output9.png",
        "../test_files/output10.png"
    };

    for (int i = 0; i < 10; i++)
        imageLoader.saveAsImage(finalImages[i], 100, 100, outputFiles[i]);
    
}