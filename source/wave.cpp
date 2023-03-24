#include <wave.hpp>
#include <utils/array2D.hpp>
#include <vector>
#include <tuple>
#include <cmath>
#include <time.h> // Used in random number seed

#include <iostream>


namespace wfc {

    Tile::Tile(const input::RuleSet &rules) :
        states_(rules.numStates(), true),
        collapsed_(false), finalState_(false)
    {
        updateEntropy(rules);
    }

    void Tile::reset(const input::RuleSet &rules) {
        collapsed_ = false;
        states_.setAll(true);
        updateEntropy(rules);
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

    void Tile::collapse(const input::RuleSet &rules, Coords position) {
        if (collapsed_) return;
        // printTile();
        collapsed_ = true;
        // Generate random number in range 0-weightSum
        double number = ((double)(rand()) / (RAND_MAX+1)) * weightSum_;
        // std::cout << ' ' << number << ' ' << weightSum_ << ' ';
        // Find which state random number falls under
        int i = -1;
        do {
            i++;
            if (states_.bit(i)) {
                number -= rules.getWeight(i);
                states_.setBit(i, false);
            }
        } while (number >= 0);
        // std::cout << i << ' ' << number << ' ' << position.x << ',' << position.y << '\n'; 
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
            // std::cout << "Propagating Tile: " << target.coords.x << ", " << target.coords.y << '\n';
            waveGrid_[target.coords.y][target.coords.x]
                ->localPropagate(target.coords, waveGrid_, rules_,
                                 queue, target.direction);
            // if (queue.size() > 500) 
            //     std::cout << "Queue Length: " << queue.size() << '\n';
        }
        // std::cout << '\n';


    }

    void Tile::enforceRule(Coords position, Array2D<Tile*> &waveGrid,
            const input::RuleSet &rules, DynamicBitset &enforcedRule,
            PropagationQueue<PropagationTarget, QUEUE_SIZE> &queue,
            WaveDirection direction) {
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
        if (!changed) return;
        // std::cout << '\n';
        // std::cout << "finished checking for change : " << (int)changed << " - " << std::flush;
        states_ &= enforcedRule;
        
        // std::cout << "changed state and end of enforcing rule: " << changed << "\n";
        // std::cout << "post enforce test\n";
        updateEntropy(rules);
        queue.push({position, direction});
    }

    void Tile::localPropagate(Coords position, Array2D<Tile*> &waveGrid,
            const input::RuleSet &rules,
            PropagationQueue<PropagationTarget, QUEUE_SIZE> &queue, 
            WaveDirection direction) {
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
        if (position.y < waveGrid.yLen()-1 )
            waveGrid[position.y+1][position.x]->enforceRule({position.x, position.y+1}, waveGrid,
                                                rules, ruleDown, queue, WaveDirection::DOWN);
        // Enforce rule up
        if ( position.y > 0)
            waveGrid[position.y-1][position.x]->enforceRule({position.x, position.y-1}, waveGrid,
                                               rules, ruleUp, queue, WaveDirection::UP);
        // Enforce Rule Right
        if (position.x < waveGrid.xLen()-1)
            waveGrid[position.y][position.x+1]->enforceRule({position.x+1, position.y}, waveGrid,
                                                rules, ruleRight, queue, WaveDirection::RIGHT);
        if (position.x > 0)
            waveGrid[position.y][position.x-1]->enforceRule({position.x-1, position.y}, waveGrid,
                                                rules, ruleLeft, queue, WaveDirection::LEFT);
        
        
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
    }

    // Builds wave grid based on provided rules
    void Wave::initialize(const input::RuleSet *rules, bool ownership) {
        if (initialized_) {
            std::cerr << "INITIALIZATION WARNING: ALREADY INITIALIZED\n";
            return;
        }
        for (int y = 0; y < waveGrid_.yLen(); y++) {
            for (int x = 0; x < waveGrid_.xLen(); x++) {
                waveGrid_[y][x] = new Tile(*rules);
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
        waveGrid_[position.y][position.x]->collapse(*rules_, position);
        Tile::propagate(position, waveGrid_, *rules_);
    }

    void Wave::collapseLowestEntropy() {
        collapseTile(lowestEntropy());
    }

    // collapse lowest entropy tile until full grid is collapsed or collision occurs
    bool Wave::collapse() {
        Tile::propagate({0,0}, waveGrid_, *rules_);
        if (collapsed_) return false;
        while (!collapsed_) {
            collapseLowestEntropy();
        }

        return true;
    }

    bool Wave::reset() {
        collapsed_ = false;
        for (int y = 0; y < waveGrid_.yLen(); y++) {
            for (int x = 0; x < waveGrid_.xLen(); x++) {
                waveGrid_[y][x]->reset(*rules_);
            }
        }
    }

    input::WaveGrid* Wave::saveToWaveGrid() const {
        input::WaveGrid *output = new input::WaveGrid(waveGrid_.xLen(), waveGrid_.yLen());
        for (int x = 0; x < waveGrid_.xLen(); x++) {
            for (int y = 0; y < waveGrid_.yLen(); y++) {
                if (waveGrid_[y][x]->collapsed())
                    output->setTile(x, y, waveGrid_[y][x]->finalState()+1);
                else 
                    output->setTile(x, y, 0);
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