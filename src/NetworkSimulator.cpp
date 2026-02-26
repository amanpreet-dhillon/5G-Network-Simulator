#include "NetworkSimulator.h"
#include <iostream>
#include <string>
#include "Utils.h"
#include <chrono>
#include <thread>
#include <iomanip>

NetworkSimulator::NetworkSimulator(){

    //establish Core Network pointer
    coreNetwork = new CoreNetwork();
    simulationOn = false;
    takenCoords.insert({0,0});

}

NetworkSimulator::~NetworkSimulator(){
   
    UEObservers.clear();
    gNBObservers.clear();
    coreNetwork = nullptr;
}

void NetworkSimulator::startSimulation(int totalTicks){
    
    if (!simulationOn){
        simulationOn = true;

        //setup UEs and gNBs
        setupSimulation();

        //turn on all UEs
        turnOnUEs();

        //setupGrid();
        //displayGrid();

        for (int currTick = 0; currTick < totalTicks; currTick++){

            displayGrid();

            for(UE* ue : UEObservers){
                if (ue){

                    int isGoingToInternet = Utils::generateRandNum(0, 1);
                    int destinationUE {};

                    if(isGoingToInternet == 1){ //50% chance packet is going to INTERNET
                        destinationUE = 9999;
                    } else {
                        destinationUE = 1000 + Utils::generateRandNum(1, UEObservers.size());
                    }

                    ue->generateTraffic(destinationUE, std::string("I AM A PACKET!!!!"));
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(2000));            
        }

    }
}
    

void NetworkSimulator::setupSimulation(){
   
    if(simulationOn){

        //setup gNBs
        setupgNBs();

        //setup UEs
        setupUEs();   
        
        //load UEs and gNBs to Core Network
        coreNetwork->loadEquipment(std::move(equipment_gNBs), std::string("gNB"));
        coreNetwork->loadEquipment(std::move(equipment_UEs), std::string("UE"));

        
    }
    
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
    int numUEs = Utils::generateRandNum(5,10);

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

void NetworkSimulator::setupGrid(){
    
}
    

void NetworkSimulator::displayGrid(){

    std::vector<std::vector<std::string>> grid{20, std::vector<std::string>{20, "."}};
    int offset {10};

    // Inside the grid initialization:
    for(int i=0; i<20; i++) {
        for(int j=0; j<20; j++) {
            if (i == offset && j == offset) grid[i][j] = "+"; // Origin
            else if (i == offset) grid[i][j] = "-";          // X-axis
            else if (j == offset) grid[i][j] = "|";          // Y-axis
            else grid[i][j] = ".";
        }
    }

    for (auto* gnb : gNBObservers){
        int col = gnb->getLocation().first + offset;
        int row = offset - gnb->getLocation().second;

        if (row >= 0 && row < 20 && col >= 0 && col < 20){
            grid[row][col] = "g"; //+ std::to_string(gnb->getID());
        }
    }

    for (auto* ue : UEObservers){
        int col = ue->getLocation().first + offset;
        int row = offset - ue->getLocation().second;

        if (row >= 0 && row < 20 && col >= 0 && col < 20){
            grid[row][col] = "U"; //+ std::to_string(ue->getID());
        }
    }

    grid[offset-0][0+offset] = std::string("C");

    std::cout << "\033[2J\033[;1H";
    std::cout << "\n--- NETWORK MAP: " << std::endl;

    

    for(int i = 0; i < 20; i++){
        for (int j = 0; j < 20; j++){

            std::string cell = grid[i][j];

            if(cell[0] == 'g'){ //gnb will be printed greed
                std::cout << "\033[32m" << std::left << std::setw(5) << grid[i][j];
            } else if (cell[0] == 'U'){ //UE will be printed red
                std::cout << "\033[31m" << std::left << std::setw(5) << grid[i][j];
            } else if (cell[0] =='C'){
                std::cout << "\033[33m" << std::left << std::setw(5) << grid[i][j];
            } else {
                std::cout << std::left << std::setw(5) << grid[i][j];
            }
            std::cout << "\033[0m";
        }
        std::cout << "\n";
    }
    


    
}