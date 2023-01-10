#include <wave.hpp>
#include <utils/array2D.hpp>
#include <vector>
#include <tuple>
#include <cmath>

Tile<numStates>::Tile (const double* weights, size_t numberOfStates) :
    states_(numberOfStates), weights_(weights),
    collapsed_(false), finalState_(false)
{
    updateEntropy();
}

double Tile::updateEntropy() {
    double weightSum_ = 0;
    double sumXLog = 0;

    for (int i = 0; i < numStates; i++) {
        if (states_.bit(i) == true) {
            weightSum_ += weights_[i];
            sumXLog += weights_[i] * log(weights_[i]);
        }
    }
    return entropy_ = log(weightSum_) - (sumXLog / weightSum_);
}

template <size_t numStates>
size_t Tile::collapse() {
    collapsed_ = true;

    // Generate random number in range 0-weightSum
    double number = ((double)rand() / RAND_MAX) * weightSum_;

    // Find which state random number falls under
    int i = -1;
    while (number > 0) {
        i++;
        if (states_.bit(i)) {
            number -= weights_[i];
            states_.setBit(i, false);
        }
    } 
    states_.setBit(i, true);
    finalState_ = i;
    i++;
    while (i < numStates) {
        if (states_.bit(i)
            states_.setBit(i, false);
        i++;
    }
}

void Tile::enforceRule(Wave::Coords position, Array2D<Tile*> &waveGrid_,
        std::vector<DynamicBitset[4]> &rules_, DynamicBitset &rule_) {
    bool changed = false; // if rule changes state
    if (states & (~))
}

size_t Tile::type() {
    if (collapsed_) {
        return finalState_;
    }
    return 0;
}





Wave::Wave(size_t width, size_t height, size_t states, const double* weights) :
    waveGrid_(height, width), weights_(weights)
    {
        Tile* = tile;
        for (int y = 0; y < waveGrid_.yLen) {
            for (int x = 0; x < waveGrid_.xLen) {
                tile = new Tile(weights_)
            }
        }
    }




// Returns tile with the lowest entropy in grid
Wave::Coords Wave::lowestEntropy() {

    std::vector<Coords> stack;
    double highestEntropy = -1;
    for (size_t i = 0; i < waveGrid_.yLen(); i++) {
        for (size_t j = 0; j < waveGrid_.xLen(); j++) {
            if (waveGrid_[i][j].entropy() > highestEntropy) {
                highestEntropy = waveGrid_[i][j].entropy();
                stack.clear();
                stack.push_back({i,j});
            } else if (waveGrid_[i][j] == highestEntropy) {
                stack.push_back({i,j});
            }
        }
    }
    if (stack.size() == 1) return stack[0];
    double randomNum = ((double)rand() / RAND_MAX) * stack.size();
    return stack[std::floor(randNum)];
}

// Callapse tile based on weights
bool Wave::collapseTile(Coords position) {
    
}

// Propigate any changes from collapsed tile
bool Wave::propigate() {}

// collapse lowest entropy tile until full grid is collapsed or collision occurs
bool Wave::collapse() {
    int collapsed = false;
    Coords location;
    while (!collapsed) {
        location = lowestEntropy();
        collapseTile(location);
        collapsed = !propigate();
    }
    if (collapsed = -1) return false;
    return true;
}


#include <iostream>
int main() {

    double weights[4] = {0.7, 0.3, 1, 0.1};
    double chances[4] = {0};

    int iterations = (weights[0] + weights[1] + weights[2] + weights[3]) * 10000;

    Tile<4> tile(weights);

    size_t result = 0;
    for (int i = 0; i < iterations; i++) {
        std::cout << "Iteration: " << i;
        result = tile.collapse();
        std::cout << "  -  Result: " << result << "\n";
    }


}