#include "UE.h"
#include "gNB.h"
#include "Packet.h"

UE::UE(int givenID, int xCoord, int yCoord) : Node(givenID, xCoord, yCoord){
    connected_gNB = nullptr;
    active = false;
}

UE::~UE(){
    
}

void UE::turnOn(std::vector<gNB*> gNBList){
    
    if(!this->active){
        active = true;
        float currDistance = 0.0;

        for (gNB* tower : gNBList){ //when UE turns on, find closest gNB
            if (util.calculateDistance(*this, *tower) > currDistance){
                currDistance = util.calculateDistance(*this, *tower);
                connected_gNB = tower;
            }
        }
    }

}

void UE::turnOff(){
    if(this->active){
        active = false;
        //connected_gNB.disconnectUE(this->id);
        connected_gNB = nullptr;    //when UE turns off, disconnect from gNB
    }
    
}