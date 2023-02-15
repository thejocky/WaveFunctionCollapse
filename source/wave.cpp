#include <wave.hpp>
#include <utils/array2D.hpp>
#include <vector>
#include <tuple>
#include <cmath>
#include <time.h> // Used in random number seed

#include <iostream>


namespace wfc {

    Tile::Tile(size_t numberOfStates, input::RuleSet &rules) :
        states_(numberOfStates, true),
        collapsed_(false), finalState_(false)
    {
        updateEntropy(rules);
    }

    double Tile::updateEntropy(input::RuleSet &rules) {
        weightSum_ = 0;
        double sumXLog = 0;
        if (collapsed_) {
            entropy_ = -1; 
            return -1;
        }

        for (int i = 0; i < rules.numStates(); i++) {
            if (states_.bit(i) == true && rules.getWeight(i)) {
                weightSum_ += rules.getWeight(i);
                sumXLog += rules.getWeight(i) * log(rules.getWeight(i));
            }
        }
        entropy_ = log(weightSum_) - (sumXLog / weightSum_);
        return entropy_;
    }

    void Tile::collapse(input::RuleSet &rules) {
        collapsed_ = true;
        // Generate random number in range 0-weightSum
        double number = ((double)(rand()) / RAND_MAX) * weightSum_;

        // Find which state random number falls under
        int i = -1;
        do {
            i++;
            if (states_.bit(i)) {
                number -= rules.getWeight(i);
                states_.setBit(i, false);
            }
        } while (number > 0);
        states_.setBit(i, true);
        finalState_ = i;
        while (i < rules.numStates()-2) {
            i++;
            states_.setBit(i, false);
        }
        updateEntropy(rules);
    }

    void Tile::enforceRule(Coords position, Array2D<Tile*> &waveGrid,
            input::RuleSet &rules, DynamicBitset &enforcedRule) {
        bool changed = false; // if rule changes state
        for (int i = 0; i < states_.blockSize(); i++) {
            if (states_.block(i) & (~enforcedRule.block(i))) {
                changed = true;
                break;
            }
        }

        states_ &= enforcedRule;
        
        if (changed) {
            updateEntropy(rules);
            propagate(position, waveGrid, rules);
        }
    }

    void Tile::propagate(Coords position, Array2D<Tile*> &waveGrid,
            input::RuleSet &rules) {
        DynamicBitset ruleUp(states_.size());
        DynamicBitset ruleDown(states_.size());
        DynamicBitset ruleLeft(states_.size());
        DynamicBitset ruleRight(states_.size());
        for (int i = 0; i < states_.size(); i++) {
            if (states_.bit(i)) {
                ruleUp |= rules.getRule(i, WaveDirection::UP);
                ruleDown |= rules.getRule(i, WaveDirection::DOWN);
                ruleLeft |= rules.getRule(i, WaveDirection::LEFT);
                ruleRight |= rules.getRule(i, WaveDirection::RIGHT);
            }
        }
        
        if (position.y < waveGrid.yLen()-1)
            waveGrid[position.y+1][position.x]->enforceRule({position.x, position.y+1},
                                                waveGrid, rules, ruleDown);
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



    Wave::Wave(size_t width, size_t height) :
        waveGrid_(height, width), collapsed_(false), initialized_(false)
    {
        srand(time(0));
        // for (int y = 0; y < waveGrid_.yLen(); y++) {
        //     for (int x = 0; x < waveGrid_.xLen(); x++) {
        //         waveGrid_[y][x] = new Tile(states, *rules);
        //     }
        // }
    }




    // Returns tile with the lowest entropy in grid
    Coords Wave::lowestEntropy() {

        std::vector<Coords> stack;
        #define MAX_ENTROPY 10000000
        double lowestEntropy = MAX_ENTROPY;
        for (int y = 0; y < waveGrid_.yLen(); y++) {
            for (int x = 0; x < waveGrid_.xLen(); x++) {
                if (waveGrid_[y][x]->entropy() < lowestEntropy && waveGrid_[y][x]->entropy()!=-1) {
                    lowestEntropy = waveGrid_[y][x]->entropy();
                    stack.clear();
                    stack.push_back({x,y});
                } else if (waveGrid_[y][x]->entropy() == lowestEntropy) {
                    stack.push_back({x,y});
                }
            }
        }
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
        waveGrid_[position.y][position.x]->collapse(*rules_);
        waveGrid_[position.y][position.x]->propagate(position, waveGrid_, *rules_);
    }


    // collapse lowest entropy tile until full grid is collapsed or collision occurs
    bool Wave::collapse() {
        waveGrid_[0][0]->propagate({0,0}, waveGrid_, *rules_);
        printWave();
        std::string dummy;
        std::cin >> dummy;
        if (collapsed_) return false;
        Coords location;
    
        location = lowestEntropy();
        while (!collapsed_) {
            collapseTile(location);

            location = lowestEntropy();
        }
        printWave();

        return true;
    }

    input::WaveGrid* Wave::saveToWaveGrid() const {
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
