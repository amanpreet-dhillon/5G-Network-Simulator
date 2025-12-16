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
        int generateID(const int);  //move to NetoworkSimulator
        std::pair<int,int> generateCoordinates();   //move to NetoworkSimulator
        float calculateDistance(const Node&, const Node&) const;
    
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