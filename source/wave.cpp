#include <wave.hpp>
#include <utils/array2D.hpp>
#include <vector>
#include <tuple>
#include <cmath>
#include <time.h> // Used in random number seed

#include <iostream>


namespace wfc {

    Tile::Tile(const input::RuleSet &rules, int* propagations_TMP) :
        states_(rules.numStates(), true),
        collapsed_(false), finalState_(false), propagations_TMP_(propagations_TMP)
    {
        updateEntropy(rules);
    }

    void Tile::reset() {
        collapsed_ = false;
        states_.setAll(true);
    }

    double Tile::updateEntropy(const input::RuleSet &rules) {
        // std::cout << "updating entropy " << rules.numStates() << "\n";
        weightSum_ = 0;
        double sumXLog = 0;
        if (collapsed_) {
            entropy_ = -1; 
            return -1;
        }
        // std::cout << "checked for collapse\n";

        for (int i = 0; i < rules.numStates(); i++) {
            // std::cout << "checking weight: " << i << " " << states_.bit(i) << " " << rules.getWeight(i) << '\n';
            if (states_.bit(i) == true && rules.getWeight(i)) {
                weightSum_ += rules.getWeight(i);
                sumXLog += rules.getWeight(i) * log(rules.getWeight(i));
            }
        }
        // std::cout << "calculated sums " << weightSum_ << "\n";
        entropy_ = log(weightSum_) - (sumXLog / weightSum_);
        return entropy_;
    }

    void Tile::collapse(const input::RuleSet &rules) {
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

    void Tile::propagate(Coords position, Array2D<Tile*> &waveGrid_, const input::RuleSet &rules_) {

        PropagationQueue<PropagationTarget, QUEUE_SIZE> queue;
        PropagationTarget target;
        queue.push({position, WaveDirection::NONE});
        while (!queue.empty()) {
            target = queue.pop();
            waveGrid_[target.coords.y][target.coords.x]
                ->localPropagate(target.coords, waveGrid_, rules_,
                                 queue, target.direction);
        }



    }

    void Tile::enforceRule(Coords position, Array2D<Tile*> &waveGrid,
            const input::RuleSet &rules, DynamicBitset &enforcedRule, WaveDirection direction) {
        // std::cout << "enforcing rule: " << states_.blockSize() << " - " << std::flush;
        if (collapsed_) return;
        // std::cout << "enforcing rule\n";
        bool changed = false; // if rule changes state
        for (int i = 0; i < states_.blockSize(); i++) {
            if (states_.block(i) & (~enforcedRule.block(i))) {
                // std::cout << "setting changed to true - " << std::flush;
                changed = true;
                break;
            }
        }
        // std::cout << '\n';
        // std::cout << "finished checking for change : " << (int)changed << " - " << std::flush;
        states_ &= enforcedRule;
        
        // std::cout << "changed state and end of enforcing rule: " << changed << "\n";
        // std::cout << "post enforce test\n";
        if (changed) {
            updateEntropy(rules);
            
        }
    }

    void Tile::localPropagate(Coords position, Array2D<Tile*> &waveGrid,
            const input::RuleSet &rules,
            PropagationQueue<PropagationTarget, QUEUE_SIZE> &queue, 
            WaveDirection direction = WaveDirection::NONE) {
        // std::cout << "propagation depth: " << ++*propagations_TMP_ << '\n';
        // std::cout << "propagating\n";
        DynamicBitset ruleUp(rules.numStates());
        DynamicBitset ruleDown(rules.numStates());
        DynamicBitset ruleLeft(rules.numStates());
        DynamicBitset ruleRight(rules.numStates());
        // std::cout << "created directional rules\n";

        for (int i = 0; i < states_.size(); i++) {
            if (states_.bit(i)) {
                ruleUp |= rules.getRule(i, WaveDirection::UP);
                ruleDown |= rules.getRule(i, WaveDirection::DOWN);
                ruleLeft |= rules.getRule(i, WaveDirection::LEFT);
                ruleRight |= rules.getRule(i, WaveDirection::RIGHT);
            }
        }
        // std::cout << "set directional rules\n";
        
        // Enforce rule down
        if ((direction != WaveDirection::DOWN) && position.y < waveGrid.yLen()-1 )
            waveGrid[position.y+1][position.x]->enforceRule({position.x, position.y+1},
                                                waveGrid, rules, ruleDown, WaveDirection::DOWN);
        // Enforce rule up
        if ((direction != WaveDirection::UP) && position.y > 0)
            waveGrid[position.y-1][position.x]->enforceRule({position.x, position.y-1},
                                               waveGrid, rules, ruleUp, WaveDirection::UP);
        // Enforce Rule left
        if ((direction != WaveDirection::RIGHT) && position.x < waveGrid.xLen()-1)
            waveGrid[position.y][position.x+1]->enforceRule({position.x+1, position.y},
                                                waveGrid, rules, ruleLeft, WaveDirection::LEFT);
        if ((direction != WaveDirection::LEFT) && position.x > 0)
            waveGrid[position.y][position.x-1]->enforceRule({position.x-1, position.y},
                                                waveGrid, rules, ruleRight, WaveDirection::RIGHT);
        
        
        (*propagations_TMP_)--;
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
        waveGrid_(height, width), collapsed_(false), initialized_(false), propagations_TMP_(0)
    {
        srand(time(0));
        // for (int y = 0; y < waveGrid_.yLen(); y++) {
        //     for (int x = 0; x < waveGrid_.xLen(); x++) {
        //         waveGrid_[y][x] = new Tile(states, *rules);
        //     }
        // }
    }

    // Builds wave grid based on provided rules
    void Wave::initialize(const input::RuleSet *rules, bool ownership) {
        if (initialized_) {
            std::cerr << "INITIALIZATION WARNING: ALREADY INITIALIZED\n";
            return;
        }
        for (int y = 0; y < waveGrid_.yLen(); y++) {
            for (int x = 0; x < waveGrid_.xLen(); x++) {
                waveGrid_[y][x] = new Tile(*rules, &propagations_TMP_);
            }
        }
        rules_ = rules;
        rulesOwnership_ = ownership;
        // std::cout << "initailized state: " << rules->getWeight(0);
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
        waveGrid_[position.y][position.x]->localPropagate(position, waveGrid_, *rules_);
    }



    // collapse lowest entropy tile until full grid is collapsed or collision occurs
    bool Wave::collapse() {
        // std::cout << "wave grid size: " << waveGrid_[0][0] << '\n';
        waveGrid_[0][0]->localPropagate({0,0}, waveGrid_, *rules_);
        // std::cout << "finished initial propagation\n";
        // printWave();
        // std::string dummy;
        // std::cin >> dummy;
        if (collapsed_) return false;
        Coords location;
        // std::cout << "getting entropy\n";
        location = lowestEntropy();
        while (!collapsed_) {
            // std::cout << "collapsing\n";
            collapseTile(location);
        // std::cout << "getting entropy\n";

            location = lowestEntropy();
        }
        // printWave();

        return true;
    }

    bool Wave::reset() {
        for (int y = 0; y < waveGrid_.yLen(); y++) {
            for (int x = 0; x < waveGrid_.xLen(); x++) {
                waveGrid_[y][x]->reset();
            }
        }
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


// int main() {
//     // std::cout << "starting main\n";
//     // wfc::input::ImageLoader loader;
//     // wfc::input::RuleSet rules(80);
//     // if (!rules.addImage("../test_files/grid_test.png", loader)) return 1;
//     // std::cout << "Loaded Image\n";


//     // int width, height, nrChannels;
//     // unsigned char* data;

//     // for (int state = 0; state < 6; state++) {
//     //     for (int i = 0; i < rules.getRule(state, wfc::UP).size(); i++)
//     //         std::cout << rules.getRule(state, wfc::UP).bit(i); 
//     //     std::cout <<  " : ";
//     //     for (int i = 0; i < rules.getRule(state, wfc::DOWN).size(); i++)
//     //         std::cout << rules.getRule(state, wfc::DOWN).bit(i); 
//     //     std::cout <<  " : ";
//     //     for (int i = 0; i < rules.getRule(state, wfc::LEFT).size(); i++)
//     //         std::cout << rules.getRule(state, wfc::LEFT).bit(i); 
//     //     std::cout <<  " : ";
//     //     for (int i = 0; i < rules.getRule(state, wfc::RIGHT).size(); i++)
//     //         std::cout << rules.getRule(state, wfc::RIGHT).bit(i); 
//     //     std::cout <<  "\n";
//     // }

//     // data = stbi_load(path, &width, &height, &nrChannels, 0);
//     // if (!data) {std::cerr << "ERRROR OPENING IMAGE: " << path << "\n"; return false;}
//     // auto returnVal = addImageData(data, width, height, nrChannels, loader);
//     // delete[] data;

//     // for (int i = 0; i < rules.getStates(); i++) {
//     //     std::cout << ", " << rules.getWeight(i);
//     // }

//     // wfc::Wave wave(100, 100, 80, &rules);
//     // std::cout << "created wave\n";

//     // wave.collapse();
//     // std::cout << "collapsed wave\n";

//     // wfc::input::WaveGrid *grid = wave.saveToWaveGrid(); 
//     // std::cout << "saved to wavegrid\n";
//     // // std::cout << grid << "\n";
//     // loader.saveAsImage(grid, "../test_files/output_2.png");

// }
