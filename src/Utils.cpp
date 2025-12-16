#include "Utils.h"
#include <stdlib.h>
#include <ctime>
#include <cmath>

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


std::pair<int,int> Utils::generateCoordinates(){    //Move this to NetworkSimulator
    int x {rand() % 101};
    int y {rand() % 101};

    std::pair<int, int> coordinates {x, y};

    while(!existingCoordinates.insert(coordinates).second){
        x = {rand() % 101};
        y = {rand() % 101};
        coordinates = {x, y};
    }

    return coordinates;
}
*/

float Utils::calculateDistance(const Node& UE, const Node& gNB) const {
    auto [x1, y1] = UE.getLocation();
    auto [x2, y2] = gNB.getLocation();
    
    return std::sqrt(((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)));
}