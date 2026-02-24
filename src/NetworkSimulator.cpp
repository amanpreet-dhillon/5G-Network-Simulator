#include "NetworkSimulator.h"
#include <iostream>
#include <string>
#include "Utils.h"

NetworkSimulator::NetworkSimulator(){

    

}

NetworkSimulator::~NetworkSimulator(){


}

void NetworkSimulator::startSimulation(){
    
    //turn on all UEs
    turnOnUEs();
}

void NetworkSimulator::setupSimulation(){

    //establish Core Network pointer
    coreNetwork = new CoreNetwork();

    //setup gNBs
    setupgNBs();

    //setup UEs
    setupUEs();   
    
    //load UEs and gNBs to Core Network
    coreNetwork->loadEquipment(std::move(equipment_gNBs), std::string("gNB"));
    coreNetwork->loadEquipment(std::move(equipment_UEs), std::string("UE"));

}

void NetworkSimulator::setupgNBs(){
    int numgNBs = Utils::generateRandNum(1,3);

    for (int i = 0; i < numgNBs; i++){

        std::pair gNBCoords = Utils::generateCoordinates();
        while (takenCoords.find(gNBCoords) != takenCoords.end()){   //keep generating new coords until a free space is generated
            gNBCoords = Utils::generateCoordinates();
        }

        takenCoords.insert(gNBCoords);

        auto gnb = std::make_unique<gNB>(++currgNBID, gNBCoords.first, gNBCoords.second);

        gnb->establishConnectionToCore(coreNetwork);

        gNBObservers.push_back(gnb.get());

        equipment_gNBs.push_back(std::move(gnb));
    }
}


void NetworkSimulator::setupUEs(){
    int numUEs = Utils::generateRandNum(1,10);

    for (int i = 0; i < numUEs; i++){

        std::pair UECoords = Utils::generateCoordinates();
        while (takenCoords.find(UECoords) != takenCoords.end()){   //keep generating new coords until a free space is generated
            UECoords = Utils::generateCoordinates();
        }

        takenCoords.insert(UECoords);

        auto ue = std::make_unique<UE>(++currUEID, UECoords.first, UECoords.second);

        UEObservers.push_back(ue.get());

        equipment_UEs.push_back(std::move(ue));
    }
}

void NetworkSimulator::turnOnUEs(){
    for (auto ue : UEObservers){
        if(ue){
            ue->turnOn(gNBObservers);
        }
    }
}