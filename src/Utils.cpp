#include "Utils.h"
#include <stdlib.h>
#include <ctime>
#include <cmath>
#include <random>

Utils::Utils(){
}

Utils::~Utils(){
}

std::pair<int,int> Utils::generateCoordinates(){
    int x {generateRandNum(-9,9)};
    int y {generateRandNum(-9,9)};

    std::pair<int, int> coordinates {x, y};
    
    return coordinates;
}


float Utils::calculateDistance(const Node& UE, const Node& gNB) const {
    auto [x1, y1] = UE.getLocation();
    auto [x2, y2] = gNB.getLocation();
    
    return std::sqrt(((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)));
}

int Utils::generateRandNum(int start, int end){
    static std::random_device rd; 
    static std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distrib(start, end); // Range [start, end]
    return distrib(gen);
}