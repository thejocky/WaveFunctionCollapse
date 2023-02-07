#include <wave.hpp>
#include <utils/array2D.hpp>
#include <vector>
#include <tuple>
#include <cmath>
#include <time.h> // Used in random number seed

#include <iostream>

// #define STB_IMAGE_IMPLEMENTATION
// #include <dep/stb/stb_image.h>

namespace wfc {

    Tile::Tile(size_t numberOfStates, input::RuleSet &rules) :
        states_(numberOfStates, true),
        collapsed_(false), finalState_(false)
    {
        updateEntropy(rules);
    }

    double Tile::updateEntropy(input::RuleSet &rules) {
        // std::cout << "\nSetting weight sum to 0\n";
        weightSum_ = 0;
        double sumXLog = 0;
        if (collapsed_) {
            entropy_ = -1; 
            return -1;
        }

        for (int i = 0; i < rules.getStates(); i++) {
            if (states_.bit(i) == true && rules.getWeight(i)) {
                weightSum_ += rules.getWeight(i);
                // std::cout << "adding to sumXlog: " << rules.getWeight(i) * log(rules.getWeight(i)) << ' ' << rules.getWeight(i) << '\n';
                sumXLog += rules.getWeight(i) * log(rules.getWeight(i));
            }
        }
        entropy_ = log(weightSum_) - (sumXLog / weightSum_);
        // if (weightSum_ == 0)
            // std::cout << "updated entropy: " << entropy_ << ' ' << weightSum_<< '\n';
        return entropy_;
    }

    void Tile::collapse(input::RuleSet &rules) {
        collapsed_ = true;

        // std::cout << "collapsing tile: " << weightSum_ << " : ";
        // for (int i = 0; i < states_.size(); i++)
        //     std::cout << states_.bit(i);
        // std::cout << " : ";

        // Generate random number in range 0-weightSum
        double number = ((double)(rand()) / RAND_MAX) * weightSum_;

        // Find which state random number falls under
        int i = -1;
        // std::cout << "index: " <<  i << " : " << number<< '\n'; 
        do {
            i++;
            if (states_.bit(i)) {
                number -= rules.getWeight(i);
                states_.setBit(i, false);
            }
        } while (number > 0);
        // std::cout << "index: " <<  i << " : " << number<< '\n'; 
        states_.setBit(i, true);
        // std::cout << i << " : " << number << " : ";
        finalState_ = i;
        while (i < rules.getStates()-1) {
            // if (states_.bit(i))
            i++;
            // std::cout << "setting bit\n";
                states_.setBit(i, false);
        }

        // for (int i = 0; i < states_.size(); i++)
        //     std::cout << states_.bit(i);
        // std::cout << "\n";
        updateEntropy(rules);
    }

    void Tile::enforceRule(Coords position, Array2D<Tile*> &waveGrid,
            input::RuleSet &rules, DynamicBitset &enforcedRule) {
        bool changed = false; // if rule changes state
        for (int i = 0; i < states_.blockSize(); i++) {
            if (states_.block(i) & (~enforcedRule.block(i))) {
                // std::cout << "\n" << states_.block(i) << " : " << (~enforcedRule.block(i)) << '\n';
                changed = true;
                break;
            }
        }
        // if (changed) {
        //     std::cout << "changed Tile: " << position.x << ',' << position.y << ' ';
        //     for (int i = 0; i < states_.size(); i++)
        //         std::cout << states_.bit(i);
        //     std::cout << " : ";
        // }
        // std::cout << "Enforcing Rule: ";
        // for (int i = 0; i < states_.size(); i++)
        //     std::cout << states_.bit(i);
        // std::cout << " : ";

        // std::cout << "checked for change - " << std::flush;
        states_ &= enforcedRule;
        // std::cout << "changed: " << changed << '\n';

        // if (changed) {
        //     for (int i = 0; i < states_.size(); i++)
        //         std::cout << states_.bit(i);
        //         std::cout << "\n";
        // }
        
        // for (int i = 0; i < enforcedRule.size(); i++)
        //     std::cout << enforcedRule.bit(i);
        // std::cout << " : ";
        // for (int i = 0; i < states_.size(); i++)
        //     std::cout << states_.bit(i);
        // std::cout << '\n';
        
        if (changed) {
            updateEntropy(rules);
            propagate(position, waveGrid, rules);
        }

        // std::cout << "finished enforcing - " << std::flush;
    }

    void Tile::propagate(Coords position, Array2D<Tile*> &waveGrid,
            input::RuleSet &rules) {
        // std::cout << "starting propagation" << position.x << ',' << position.y << '\n';
        DynamicBitset ruleUp(states_.size());
        DynamicBitset ruleDown(states_.size());
        DynamicBitset ruleLeft(states_.size());
        DynamicBitset ruleRight(states_.size());
        for (int i = 0; i < states_.size(); i++) {
            if (states_.bit(i)) {
                // std::cout << "adding to rule: " << i << ' ' << states_.size() << " : " << rules.getRule(i, WaveDirection::UP).bit(i) << "\n";
                ruleUp |= rules.getRule(i, WaveDirection::UP);
                // std::cout << "added to up rule\n";
                ruleDown |= rules.getRule(i, WaveDirection::DOWN);
                ruleLeft |= rules.getRule(i, WaveDirection::LEFT);
                ruleRight |= rules.getRule(i, WaveDirection::RIGHT);
            }
        }
        // for (int i = 0; i < ruleUp.size(); i++)
        //     std::cout << ruleUp.bit(i);
        // std::cout << '\n';
        // for (int i = 0; i < ruleDown.size(); i++)
        //     std::cout << ruleDown.bit(i);
        // std::cout << '\n';
        // for (int i = 0; i < ruleLeft.size(); i++)
        //     std::cout << ruleLeft.bit(i);
        // std::cout << '\n';
        // for (int i = 0; i < ruleRight.size(); i++)
        //     std::cout << ruleRight.bit(i);
        // std::cout << '\n';
        // for (int i = 0; i < rules.getRule(0, WaveDirection::UP).size(); i++)
        //     std::cout << rules.getRule(0, WaveDirection::UP).bit(i);
        // std::cout << '\n';
        
        // std::cout << "enforcing rules: " << waveGrid.yLen() << "\n";
        
        if (position.y < waveGrid.yLen()-1)
            waveGrid[position.y+1][position.x]->enforceRule({position.x, position.y+1},
                                                waveGrid, rules, ruleDown);
        // std::cout << "cenforced rule up-" << std::flush;
        
        if (position.y > 0)
            waveGrid[position.y-1][position.x]->enforceRule({position.x, position.y-1},
                                                waveGrid, rules, ruleUp);
        if (position.x > 0)
            waveGrid[position.y][position.x-1]->enforceRule({position.x-1, position.y},
                                                waveGrid, rules, ruleRight);
        if (position.x < waveGrid.xLen()-1)
            waveGrid[position.y][position.x+1]->enforceRule({position.x+1, position.y},
                                                waveGrid, rules, ruleLeft);
    }

    size_t Tile::finalState() {
        if (collapsed_) {
            return finalState_;
        }
        return 0;
    }

    void Tile::printTile() {
        if (collapsed_)
            std::cout << "\033[1;32m"; // Set to bold Green for colapsed tile
        for (int i = 0; i < states_.size(); i++)
            std::cout << states_.bit(i);
        if (collapsed_)
            std::cout << "\033[0m"; // Set back to default text if changed
    }



    Wave::Wave(size_t width, size_t height, size_t states, input::RuleSet *rules) :
        waveGrid_(height, width), rules_(rules), collapsed_(false)
    {
        srand(time(0));
        for (int y = 0; y < waveGrid_.yLen(); y++) {
            for (int x = 0; x < waveGrid_.xLen(); x++) {
                waveGrid_[y][x] = new Tile(states, *rules);
            }
        }
    }




    // Returns tile with the lowest entropy in grid
    Coords Wave::lowestEntropy() {

        std::vector<Coords> stack;
        #define MAX_ENTROPY 10000000
        double lowestEntropy = MAX_ENTROPY;
        for (int y = 0; y < waveGrid_.yLen(); y++) {
            for (int x = 0; x < waveGrid_.xLen(); x++) {
                // std::cout << "Checking tile: " << x << ", " << y << " : " << waveGrid_[y][x]->entropy() << "\n";
                if (waveGrid_[y][x]->entropy() < lowestEntropy && waveGrid_[y][x]->entropy()!=-1) {
                    lowestEntropy = waveGrid_[y][x]->entropy();
                    stack.clear();
                    stack.push_back({x,y});
                } else if (waveGrid_[y][x]->entropy() == lowestEntropy) {
                    stack.push_back({x,y});
                }
            }
        }
        // std::cout << "finished checking tiles : ";
        // std::cout << lowestEntropy << ' ' << stack.size() << "\n";
        if (lowestEntropy == MAX_ENTROPY) {
            collapsed_ = true;
            return {0,0};
        }
        if (stack.size() == 1) return stack[0];
        
        double randomNum = ((double)rand() / RAND_MAX) * stack.size();
        return stack[std::floor(randomNum)];
    }

    // Callapse tile based on weights
    bool Wave::collapseTile(Coords position) {
        // std::cout << position.x << ", " << position.y << " - collapsing-" << std::flush;
        waveGrid_[position.y][position.x]->collapse(*rules_);
        // std::cout << "finished : propagating-" << std::flush;
        waveGrid_[position.y][position.x]->propagate(position, waveGrid_, *rules_);
        // std::cout << "finished";
    }


    // collapse lowest entropy tile until full grid is collapsed or collision occurs
    bool Wave::collapse() {
        waveGrid_[0][0]->propagate({0,0}, waveGrid_, *rules_);
        printWave();
        std::string dummy;
        std::cin >> dummy;
        // std::cout << "state of collapsed: " << collapsed_ << '\n';
        if (collapsed_) return false;
        Coords location;
        
        // std::cout << "Getting lowest entrop\n";
        location = lowestEntropy();
        while (!collapsed_) {
            // std::cout << "collapsing Tile : " << std::flush;
            collapseTile(location);

            // std::cout << "Getting lowest entrop\n";
            location = lowestEntropy();
        }
        printWave();

        // std::cout << "Finished Collapsing\n";
        return true;
    }

    input::WaveGrid* Wave::saveToWaveGrid() {
        if (!collapsed_) return nullptr;
        input::WaveGrid *output = new input::WaveGrid(waveGrid_.xLen(), waveGrid_.yLen());
        for (int x = 0; x < waveGrid_.xLen(); x++) {
            for (int y = 0; y < waveGrid_.yLen(); y++) {
                output->setTile(x, y, waveGrid_[y][x]->finalState()+1);
            }
        }
        return output;
    }

    void Wave::printWave() {
        for (int y = 0; y < waveGrid_.yLen(); y++) {
            for (int x = 0; x < waveGrid_.xLen(); x++) {
                waveGrid_[y][x]->printTile();
                std::cout << ' ';
            }
            std::cout << '\n';
        }
    }   

}


int main() {
    std::cout << "starting main\n";
    wfc::input::ImageLoader loader;
    wfc::input::RuleSet rules(80);
    if (!rules.addImage("../test_files/grid_test.png", loader)) return 1;
    std::cout << "Loaded Image\n";


    // int width, height, nrChannels;
    // unsigned char* data;

    // for (int state = 0; state < 6; state++) {
    //     for (int i = 0; i < rules.getRule(state, wfc::UP).size(); i++)
    //         std::cout << rules.getRule(state, wfc::UP).bit(i); 
    //     std::cout <<  " : ";
    //     for (int i = 0; i < rules.getRule(state, wfc::DOWN).size(); i++)
    //         std::cout << rules.getRule(state, wfc::DOWN).bit(i); 
    //     std::cout <<  " : ";
    //     for (int i = 0; i < rules.getRule(state, wfc::LEFT).size(); i++)
    //         std::cout << rules.getRule(state, wfc::LEFT).bit(i); 
    //     std::cout <<  " : ";
    //     for (int i = 0; i < rules.getRule(state, wfc::RIGHT).size(); i++)
    //         std::cout << rules.getRule(state, wfc::RIGHT).bit(i); 
    //     std::cout <<  "\n";
    // }

    // data = stbi_load(path, &width, &height, &nrChannels, 0);
    // if (!data) {std::cerr << "ERRROR OPENING IMAGE: " << path << "\n"; return false;}
    // auto returnVal = addImageData(data, width, height, nrChannels, loader);
    // delete[] data;

    // for (int i = 0; i < rules.getStates(); i++) {
    //     std::cout << ", " << rules.getWeight(i);
    // }

    wfc::Wave wave(100, 100, 80, &rules);
    std::cout << "created wave\n";

    wave.collapse();
    std::cout << "collapsed wave\n";

    wfc::input::WaveGrid *grid = wave.saveToWaveGrid(); 
    std::cout << "saved to wavegrid\n";
    // std::cout << grid << "\n";
    loader.saveAsImage(grid, "../test_files/output_2.png");

}
