#include <wfc.hpp>

#include <iostream>
// #include<windows.h>

int main () {

    std::cout << "Creating image loader and rule builder\n";

    uint8_t* finalImages[10] = {nullptr};

    wfc::input::ImageLoader imageLoader;
    wfc::input::RuleSetBuilder ruleBuilder;

    std::cout << "adding image to rules\n";

    // Convert image into the ruleset
    ruleBuilder.addImage("../test_files/input_2.png", imageLoader);
    
    std::cout << "Creating wave\n";
    wfc::Wave wave(100, 100);
    wave.initialize(ruleBuilder.referenceRuleSet());
// std::string dummy;
    // while (!wave.collapsed()) {
    //     for (int i = 0; i < 50; i++) {
    //         // std::cout << "collapsing lowest entropy\n";
    //         wave.collapseLowestEntropy();
    //     }
    //     std::cout << "Saving as image\n";
    //     imageLoader.saveAsImage(wave, "../test_files/output1.png");
    //     // std::cin >> dummy;
    //     // Sleep(10);

    // }
    // wave.printWave();
    // std::cout << "green rule down for sky: " << ruleBuilder.referenceRuleSet()->getRule(6, wfc::WaveDirection::RIGHT).bit(0) << '\n'; 
    // std::cout << "Saving as image\n";
    //     imageLoader.saveAsImage(wave, "../test_files/output1.png");

    // input::WaveGrid* 
    // std::cout << "collapsing wave\n";
    // for (int i = 1; i < 10; i++) {
        // std::cout << "collapsing iteration\n";
        wave.collapse();
        // std::cout << "converting to image\n";
        // finalImages[i] = imageLoader.convertToImage(wave);
        // std::cout << "reseting\n";
    //     wave.reset();
    // }

    // std::cout << "saving images\n";

    // const char *outputFiles[10] = {
    //     "../test_files/output1.png",
    //     "../test_files/output2.png",
    //     "../test_files/output3.png",
    //     "../test_files/output4.png",
    //     "../test_files/output5.png",
    //     "../test_files/output6.png",
    //     "../test_files/output7.png",
    //     "../test_files/output8.png",
    //     "../test_files/output9.png",
    //     "../test_files/output10.png"
    // };

    // for (int i = 1; i < 10; i++) {
    //     imageLoader.saveAsImage(finalImages[i], 20, 20, outputFiles[i]);
    // }
    
    imageLoader.saveAsImage(wave, "../test_files/output1.png");

    std::cout << "num states: " << ruleBuilder.referenceRuleSet()->getRule(0, wfc::WaveDirection::DOWN).size() << '\n';

   
    // std::cout << "finished\n";
}