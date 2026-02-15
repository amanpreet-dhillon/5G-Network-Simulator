#include "UE.h"
#include "gNB.h"
#include "Packet.h"
#include "Tester.h"
#include <iostream>
#include <limits>

UE::UE(int givenID, int xCoord, int yCoord) : Node(givenID, xCoord, yCoord){
    connected_gNB = nullptr;
    active = false;
    receievedPacketSeq = 1;
    sentPacketSeq = 0;
}

UE::~UE(){
    
}

void UE::turnOn(std::vector<gNB*> gNBList){  //testing change
    
    if(!this->active){
        active = true;
        float currDistance = std::numeric_limits<float>::max();

        for (gNB* tower : gNBList){ //when UE turns on, find closest gNB //testing change
            if (util.calculateDistance(*this, *tower) < currDistance){
                currDistance = util.calculateDistance(*this, *tower);
                connected_gNB = tower;
            }
        }

        if(connected_gNB){  //ADD error handling if no gnb is connected
            connected_gNB->connectUE(this); //just for testing, remove after
            std::cout << "connected to gNB# " << connected_gNB->getID() << std::endl;
            setupRegistrationReq();
        }
    }

    

}

void UE::turnOff(){
    if(this->active){
        active = false;
        connected_gNB->disconnectUE(this);
        connected_gNB = nullptr;    //when UE turns off, disconnect from gNB
    }
    
}

 
void UE::recievePacket(std::unique_ptr<Packet> recievedPacket){    //recieve packets and check for ACK


    //printing packet (for testing mostly)
    std::cout << recievedPacket->print() << std::endl;


    if(recievedPacket->getPacketType() == PacketType::ACK){     //got ACK, remove from retransmissionQueue
        
        if(retransmissionQueue.find(recievedPacket->getSequenceNum()) != retransmissionQueue.end()){    //check if the packet exists in retransmissionQueue
            retransmissionQueue.erase(recievedPacket->getSequenceNum());
        }

    } else if (recievedPacket->getPacketType() == PacketType::SKIP){    //gNB could not find specified packet, assume it is lost and move to next sequence
        
        receievedPacketSeq++;
        bufferCleanUp(receievedPacketSeq);

    } else if (recievedPacket->getPacketType() == PacketType::NACK){    //gNB is requesting for a missing packet, send if found, else tell it to skip 
        
        if (retransmissionQueue.find(recievedPacket->getSequenceNum()) != retransmissionQueue.end()){
            std::unique_ptr<Packet> packetToSend = std::make_unique<Packet>(*retransmissionQueue[recievedPacket->getSequenceNum()]); //copy via copy constructor
            connected_gNB->recievePacket(std::move(packetToSend));
        } else {
            sendPacket(connected_gNB->getID(), -1, PacketType::SKIP, std::string("Could not find Packet #" + std::to_string(recievedPacket->getSequenceNum())), 1);
            sentPacketSeq = recievedPacket->getSequenceNum()+1;
        }


    } else if(recievedPacket->getPacketType() == PacketType::DATA) {    //recieved a DATA packet, check and verify seq
            
        if (recievedPacket->getSequenceNum() == receievedPacketSeq){   //got expected packet, send ACK, update the seq for that source
            
            sendPacket(connected_gNB->getID(), receievedPacketSeq, PacketType::ACK, std::string("Recieved Packet #" + std::to_string(receievedPacketSeq) + " from source #" + std::to_string(recievedPacket->getSource())), 1); //send ACk for packet recieved

            //include logger here
            receievedPacketSeq++;   //increment expected counter
            bufferCleanUp(receievedPacketSeq);
        
        } else if (recievedPacket->getSequenceNum() < receievedPacketSeq){    //recieved an old/duplicate packet, send ACK only
        
            sendPacket(connected_gNB->getID(), recievedPacket->getSequenceNum(), PacketType::ACK, std::string("Acknowledging duplicate Packet #" + std::to_string(recievedPacket->getSequenceNum()) + " from source #" + std::to_string(recievedPacket->getSource())), 1); //send ACk for old/dupe packet recieved
        
        } else if (recievedPacket->getSequenceNum() > receievedPacketSeq){     //recieved a future/out-of-order packet, store it and ask for missing packets
            
            buffer[recievedPacket->getSequenceNum()] = std::move(recievedPacket);    //move 'future' packet to buffer
            sendPacket(connected_gNB->getID(), receievedPacketSeq, PacketType::NACK, std::string("Did not recieve expected Packet, resend Packet #" + std::to_string(receievedPacketSeq)), 1);     //ping gNB to send expectedSeq packet, using source of packet as destination

        }
        
    } else if (recievedPacket->getPacketType() == PacketType::REGISTRATION_COMPLETE) {   //registration COMPLETE by core network, confirm using ACK 
        sendPacket(CORE_NETWORK, 0, PacketType::REGISTRATION_ACK, std::string("Registration Acknowledged by UE #" + std::to_string(this->getID())), 1);
    }


}

void UE::sendPacket(int destinationID, int expectedSeq, PacketType packetType, const std::string& data, int prio=0){

    if(packetType == PacketType::ACK){  //just sending ACK, no need to store

        std::unique_ptr<Packet> packetToSend = Packet::createPacket(this->getID(), destinationID, expectedSeq, packetType, data, prio); 
        connected_gNB->recievePacket(std::move(packetToSend));   

    } else if (packetType == PacketType::NACK) {    //sending NACK
        
        std::unique_ptr<Packet> packetToSend = Packet::createPacket(this->getID(), destinationID, expectedSeq, packetType, data, prio); 
        connected_gNB->recievePacket(std::move(packetToSend));   

    } else if(packetType == PacketType::SKIP) {     //sending SKIP
        
        std::unique_ptr<Packet> packetToSend = Packet::createPacket(this->getID(), destinationID, expectedSeq, packetType, data, prio);
        connected_gNB->recievePacket(std::move(packetToSend));  
        
    }else if (packetType == PacketType::DATA) {
        
        sentPacketSeq++;
        std::unique_ptr<Packet> packetToSend = Packet::createPacket(this->getID(), destinationID, sentPacketSeq, packetType, data, prio);     //packet to be sent
        std::unique_ptr<Packet> packetToStore = Packet::createPacket(this->getID(), destinationID, sentPacketSeq, packetType, data, prio);    //packet for storage
        
        //std::cout << packetToSend->print();

        retransmissionQueue[sentPacketSeq] = std::move(packetToStore);   

        connected_gNB->recievePacket(std::move(packetToSend));

    } else if (packetType == PacketType::REGISTRATION_REQUEST or packetType == PacketType::REGISTRATION_ACK){
        std::unique_ptr<Packet> registrationPacket = Packet::createPacket(this->getID(), CORE_NETWORK, 0, packetType, data, 1);   //destination 0 is the core network
        //std::cout << registrationReq->print();

        connected_gNB->recievePacket(std::move(registrationPacket));
    }
}

void UE::bufferCleanUp(int currSeq){

    if(!buffer.empty()){   //check if recieved packet is in buffer
                    
        if(buffer.find(receievedPacketSeq) != buffer.end()){
            while(buffer.find(receievedPacketSeq) != buffer.end()){
                sendPacket(connected_gNB->getID(), receievedPacketSeq, PacketType::ACK, std::string("Recieved Packet #" + std::to_string(receievedPacketSeq) + " from source #" + std::to_string(buffer[receievedPacketSeq]->getSource())), 1); //send ACk for packet recieved
                buffer.erase(receievedPacketSeq); //remove acknowledged packet from buffer
                receievedPacketSeq++; 
            }
        } else {    //still some missing packets before buffer can resolve
            sendPacket(connected_gNB->getID(), receievedPacketSeq, PacketType::NACK, std::string("Did not recieve expected Packet, resend Packet #" + std::to_string(receievedPacketSeq)), 1);
        }
        
    }
}

void UE::setupRegistrationReq(){    //UE sends registration req to core network containing information about UE (ID and location)
    
    std::string UEinfo = {std::to_string(this->getID()) + "," + std::to_string(this->getLocation().first) + "," + std::to_string(this->getLocation().second)};

    sendPacket(CORE_NETWORK, 0, PacketType::REGISTRATION_REQUEST, UEinfo, 1);
}


void UE::tester_addPacketToReQueue(int ue_id, int seq){

    retransmissionQueue[seq] = std::move(std::make_unique<Packet>(1005, 1001, seq, PacketType::DATA, "queued packet #" + std::to_string(seq), 0));
    
}