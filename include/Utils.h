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
        static int currUE_ID;
        static int curr_gNB_ID;
        static std::vector<int> UE_IDs;
        static std::vector<int> gNB_IDs;
        static std::set<std::pair<int, int>> existingCoordinates;

        
        
};

#endif