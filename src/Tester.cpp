#include "Tester.h"
#include <iostream>
#include "UE.h"

Tester::Tester(int givenID, int xCoord, int yCoord) : Node(givenID, xCoord, yCoord){

}

Tester::~Tester(){

}

void Tester::disconnectUE(int UEid){
    std::cout << "Disconneting UE#" << UEid << std::endl;
}

void Tester::recievePacket(std::unique_ptr<Packet> incomingPacket){
    std::cout << '\n' << incomingPacket->print() << std::endl;
    
    
}

void Tester::connectUE(UE* ueToConnect){
    connectedUE = ueToConnect;
    std::cout << "Connected to UE#" << connectedUE->getID() << std::endl;
}

void Tester::sendPacket(int dest, int seq, PacketType t, const std::string& data, int prio=0){
     std::unique_ptr<Packet> packetToSend = Packet::createPacket(this->getID(), dest, seq, t, data, prio);
     connectedUE->recievePacket(std::move(packetToSend));
}

