#include "gNB.h"
#include "UE.h"
#include <iostream>
#include <algorithm>

gNB::gNB(int givenID, int xcord, int ycord) : Node(givenID, xcord, ycord){

}

gNB::~gNB(){

}

void gNB::connectUE(UE* ue){
    
    int dlTEID = 40000 + ue->getID();
    dl_TEIDs[dlTEID] = ue->getID();
    //coreNetwork->recieveDLTEID(dlTEID, ue->getID());

    connectedUEs.push_back(ue);
}

void gNB::disconnectUE(UE* ue){
    auto UElocation = std::find(connectedUEs.begin(), connectedUEs.end(), ue);
    
    if (UElocation != connectedUEs.end()){
        connectedUEs.erase(UElocation);
    }
}

void gNB::recievePacket(std::unique_ptr<Packet> data){   //this will come from a UE, wrap, forward to Core Network

   auto wrapped = std::make_unique<GTPPacket>();
   wrapped->TEID = 80000 + data->getSource();    
   wrapped->payload = std::move(data);
   
   forwardToCore(std::move(wrapped));

}

void gNB::recievePacket(std::unique_ptr<GTPPacket> data){    //this will come from Core Netwwork, unwrap, send to destination UE

}

void gNB::forwardToCore(std::unique_ptr<GTPPacket> payload){    
    //coreNetwork->recievePacket(std::move(payload))
}

void gNB::establishConnectionToCore(const CoreNetwork* cN){
    coreNetwork = cN;
}

void gNB::sendPacket(int destID, int seq, PacketType pType, const std::string& data, int prio=0){

}

void gNB::recieveULTEID(int ul_TEID, int ueID){
    ul_TEIDs[ueID] = ul_TEID; 
}
