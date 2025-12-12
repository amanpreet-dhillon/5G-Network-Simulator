#ifndef UTILS_H
#define UTILS_H

#include "Node.h"
#include <vector>
#include <utility>
#include <set>

class Utils{

    public:
        Utils();
        int generateID(const int);
        std::pair<int,int> generateCoordinates();
        float calculateDistance(const Node&, const Node&) const;
    
    private:
        int currUE_ID;
        int curr_gNB_ID;
        std::vector<int> UE_IDs;
        std::vector<int> gNB_IDs;
        std::set<std::pair<int, int>> existingCoordinates;

        
        
};

#endif