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
        static std::pair<int,int> generateCoordinates();
        float calculateDistance(const Node&, const Node&) const;
        static int generateRandNum(int, int);
        
};

#endif