#ifndef UTILS_H
#define UTILS_H

#include "Node.h"
#include <vector>
#include <utility>
#include <set>

class Utils{

    public:
        Utils();
        ~Utils();
        static int generateID(const int);  //move to NetoworkSimulator
        static std::pair<int,int> generateCoordinates();   //move to NetoworkSimulator
        float calculateDistance(const Node&, const Node&) const;
        static int generateRandNum(int, int);
    
    private:
        /*
        static int currUE_ID;   //remove
        static int curr_gNB_ID; //remove
        static std::vector<int> UE_IDs; //remove
        static std::vector<int> gNB_IDs;    //remove
        static std::set<std::pair<int, int>> existingCoordinates;   //remove
        */
        
        
};

#endif