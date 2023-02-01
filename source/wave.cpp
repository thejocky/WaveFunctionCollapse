#include <wave.hpp>
#include <utils/array2D.hpp>
#include <vector>
#include <tuple>
#include <cmath>

#include <iostream>

namespace wfc {

    Tile::Tile(size_t numberOfStates, input::RuleSet &rules) :
        states_(numberOfStates),
        collapsed_(false), finalState_(false)
    {
        updateEntropy(rules);
    }

    double Tile::updateEntropy(input::RuleSet &rules) {
        double weightSum_ = 0;
        double sumXLog = 0;

        for (int i = 0; i < rules.getStates(); i++) {
            if (states_.bit(i) == true) {
                weightSum_ += rules.getWeight(i);
                sumXLog += rules.getWeight(i) * log(rules.getWeight(i));
            }
        }
        return entropy_ = log(weightSum_) - (sumXLog / weightSum_);
    }

    void Tile::collapse(input::RuleSet &rules) {
        collapsed_ = true;

        // Generate random number in range 0-weightSum
        double number = ((double)rand() / RAND_MAX) * weightSum_;

        // Find which state random number falls under
        int i = -1;
        while (number > 0) {
            i++;
            if (states_.bit(i)) {
                number -= rules.getWeight(i);
                states_.setBit(i, false);
            }
        } 
        states_.setBit(i, true);
        finalState_ = i;
        i++;
        while (i < rules.getStates()) {
            if (states_.bit(i))
                states_.setBit(i, false);
            i++;
        }
    }

    void Tile::enforceRule(Coords position, Array2D<Tile*> &waveGrid,
            input::RuleSet &rules, DynamicBitset &enforcedRule) {
        bool changed = false; // if rule changes state
        if (states_ & (~enforcedRule)) changed = true;
        states_ &= enforcedRule;
        if (changed) propagate(position, waveGrid, rules);
    }

    void Tile::propagate(Coords position, Array2D<Tile*> &waveGrid,
            input::RuleSet &rules) {
        DynamicBitset ruleUp(states_.size());
        DynamicBitset ruleDown(states_.size());
        DynamicBitset ruleLeft(states_.size());
        DynamicBitset ruleRight(states_.size());
        for (int i = 0; i < states_.size(); i++) {
            ruleUp |= rules.getRule(i, WaveDirection::UP);
            ruleDown |= rules.getRule(i, WaveDirection::DOWN);
            ruleLeft |= rules.getRule(i, WaveDirection::LEFT);
            ruleRight |= rules.getRule(i, WaveDirection::RIGHT);
        }
        
        if (position.y < waveGrid.yLen()-1)
            waveGrid[position.x][position.y+1]->enforceRule({},
                                                waveGrid, rules, ruleUp);
        if (position.y > 0)
            waveGrid[position.x][position.y-1]->enforceRule(position,
                                                waveGrid, rules, ruleDown);
        if (position.x > 0)
            waveGrid[position.x][position.y-1]->enforceRule(position,
                                                waveGrid, rules, ruleLeft);
        if (position.x < waveGrid.xLen()-1)
            waveGrid[position.x][position.y+1]->enforceRule(position,
                                                waveGrid, rules, ruleRight);
    }

    size_t Tile::finalState() {
        if (collapsed_) {
            return finalState_;
        }
        return 0;
    }





    Wave::Wave(size_t width, size_t height, size_t states, input::RuleSet *rules) :
        waveGrid_(height, width), rules_(rules)
    {
        for (int y = 0; y < waveGrid_.yLen(); y++) {
            for (int x = 0; x < waveGrid_.xLen(); x++) {
                waveGrid_[y][x] = new Tile(states, *rules);
            }
        }
    }




    // Returns tile with the lowest entropy in grid
    Coords Wave::lowestEntropy() {

        std::vector<Coords> stack;
        double highestEntropy = -1;
        for (int i = 0; i < waveGrid_.yLen(); i++) {
            for (int j = 0; j < waveGrid_.xLen(); j++) {
                if (waveGrid_[i][j]->entropy() > highestEntropy) {
                    highestEntropy = waveGrid_[i][j]->entropy();
                    stack.clear();
                    stack.push_back({i,j});
                } else if (waveGrid_[i][j]->entropy() == highestEntropy) {
                    stack.push_back({i,j});
                }
            }
        }
        if (stack.size() == 1) return stack[0];
        double randomNum = ((double)rand() / RAND_MAX) * stack.size();
        if (highestEntropy == 0) collapsed_ = true;
        return stack[std::floor(randomNum)];
    }

    // Callapse tile based on weights
    bool Wave::collapseTile(Coords position) {
        waveGrid_[position.y][position.x]->collapse(*rules_);
        waveGrid_[position.y][position.x]->propagate(position, waveGrid_, *rules_);
    }


    // collapse lowest entropy tile until full grid is collapsed or collision occurs
    bool Wave::collapse() {
        if (collapsed_) return false;
        Coords location;
        location = lowestEntropy();
        while (!collapsed_) {
            collapseTile(location);
            location = lowestEntropy();
        }
        return true;
    }

}

int main() {

    wfc::input::ImageLoader loader;
    wfc::input::RuleSet rules(128);
    if (!rules.addImage("../test_files/input_2.png", loader)) return 1;
    std::cout << "Loaded Image\n";

    


}
