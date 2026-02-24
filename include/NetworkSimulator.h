#ifndef NETWORKSIMULATOR_H
#define NETWORKSIMULATOR_H

#include <vector>
#include "UE.h"
#include "gNB.h"
#include "CoreNetwork.h"
#include <set>

class NetworkSimulator {
    public:
        NetworkSimulator();
        ~NetworkSimulator();
        void setupSimulation();
        void startSimulation();
        void endSimulation();

    private:
        std::vector<std::unique_ptr<Node>> equipment_UEs;
        std::vector<std::unique_ptr<Node>> equipment_gNBs;

        std::vector<UE*> UEObservers;
        std::vector<gNB*> gNBObservers;

        std::set<std::pair<int, int>> takenCoords;

        void setupgNBs();
        void setupUEs();
        void turnOnUEs();
        void turnOffUEs();


        int currgNBID = 5000;
        int currUEID = 1000;

        CoreNetwork* coreNetwork;
        //std::vector<std::vector<int>> grid(100, std::vector<int>(100,0));


};


#endif