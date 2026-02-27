#include "Utils.h"
#include <stdlib.h>
#include <ctime>
#include <cmath>
#include <random>

Utils::Utils(){

    //currUE_ID = 1000;
    //curr_gNB_ID = 5000;
    //srand(time(0));    

}

Utils::~Utils(){

}
/*
int Utils::generateID(const int type) { //move this to NetworkSimulator
    if(type == 0){  //UE ID
        UE_IDs.push_back(++currUE_ID);
        return currUE_ID;
    } else if (type == 1) { //gNB ID
        gNB_IDs.push_back(++curr_gNB_ID);
        return curr_gNB_ID;
    }

    return 0;
    
}
*/

std::pair<int,int> Utils::generateCoordinates(){    //Move this to NetworkSimulator
    int x {generateRandNum(-10,9)};
    int y {generateRandNum(-10,9)};

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