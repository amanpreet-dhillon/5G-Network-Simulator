#include "UE.h"
#include "gNB.h"
#include "Packet.h"
#include <iostream>

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


void UE::recievePacket(std::unique_ptr<Packet> recievedPacket){    //recieve packets and check for ACK

    if(recievedPacket->getPacketType() == PacketType::ACK){     //got ACK, remove from retransmissionQueue
        
        if(retransmissionQueue.find({recievedPacket->getSource(), recievedPacket->getSequenceNum()}) != retransmissionQueue.end()){    //check if the packet exists in retransmissionQueue
            retransmissionQueue.erase({recievedPacket->getSource(), recievedPacket->getSequenceNum()});
        }

    } else if (recievedPacket->getPacketType() == PacketType::SKIP){    //gNB could not find speciied packet, assume it is lost and move to next sequence
        
        recievedPacketTracker[recievedPacket->getSource()] = recievedPacket->getSequenceNum() + 1;
        bufferCleanUp(recievedPacket->getSource());

    } else if(recievedPacket->getPacketType() == PacketType::DATA){    //recieved a DATA packet, check and verify seq

        //verify if it is the epxected sequence number
        int expectedSeq {recievedPacketTracker[recievedPacket->getSource()]};
        
        if (recievedPacket->getSequenceNum() == expectedSeq){   //got expected packet, send ACK, update the seq for that source
            
            sendPacket(connected_gNB->getID(), expectedSeq, PacketType::ACK, std::string("Recieved Packet#" + std::to_string(expectedSeq)), 1); //send ACk for packet recieved

            //include logger here
            recievedPacketTracker[recievedPacket->getSource()]++;   //increment expected counter
            expectedSeq = recievedPacketTracker[recievedPacket->getSource()];

            bufferCleanUp(recievedPacket->getSource());
        
        } else if (recievedPacket->getSequenceNum() < expectedSeq){    //recieved an old/duplicate packet, send ACK only
        
            sendPacket(connected_gNB->getID(), recievedPacket->getSequenceNum(), PacketType::ACK, std::string("Recieved Packet#" + std::to_string(recievedPacket->getSequenceNum())), 1); //send ACk for old/dupe packet recieved
        
        } else if (recievedPacket->getSequenceNum() > expectedSeq){     //recieved a future/out-of-order packet, store it and ask for missing packets
            
            buffer[{recievedPacket->getSource(), recievedPacket->getSequenceNum()}] = std::move(recievedPacket);    //move 'future' packet to buffer
            sendPacket(connected_gNB->getID(), expectedSeq, PacketType::NACK, std::string("Did not recieve expected Packet, resend Packet#" + std::to_string(expectedSeq)), 1);     //ping gNB to send expectedSeq packet

        }
        
    }


}

void UE::sendPacket(int destinationID, int expectedSeq, PacketType packetType, const std::string& data, int prio=0){

    if(packetType == PacketType::ACK){  //just sending ACK, no need to store

        std::unique_ptr<Packet> packetToSend = Packet::createPacket(this->getID(), destinationID, expectedSeq, packetType, data, prio);
        //connected_gNB->recievePacket(std::move(packetToSend));   

    } else if (packetType == PacketType::NACK) {    //sending NACK
        
        std::unique_ptr<Packet> packetToSend = Packet::createPacket(this->getID(), destinationID, expectedSeq, packetType, data, prio);
        //connected_gNB->recievePacket(std::move(packetToSend));   

    } else {

        int seq {++destinationSeqTracker[destinationID]};   //update sequence number of destination
        
        std::unique_ptr<Packet> packetToSend = Packet::createPacket(this->getID(), destinationID, seq, packetType, data, prio);     //packet to be sent
        std::unique_ptr<Packet> packetToStore = Packet::createPacket(this->getID(), destinationID, seq, packetType, data, prio);    //packet for storage
        
        std::cout << packetToSend->print();

        retransmissionQueue[{destinationID, seq}] = std::move(packetToStore);   

        //connected_gNB->recievePacket(std::move(packetToSend));
    }
}

void UE::bufferCleanUp(int packetSource){

    int expectedSeq {recievedPacketTracker[packetSource]};

    if(!buffer.empty()){   //check if recieved packet is in buffer
                    
        if(buffer.find({packetSource, expectedSeq}) != buffer.end()){
            while(buffer.find({packetSource, expectedSeq}) != buffer.end()){
                sendPacket(connected_gNB->getID(), expectedSeq, PacketType::ACK, std::string("Recieved Packet#" + std::to_string(expectedSeq)), 1); //send ACk for packet recieved
                buffer.erase({packetSource, expectedSeq}); //remove acknowledged packet from buffer
                recievedPacketTracker[packetSource]++; 
                expectedSeq = recievedPacketTracker[packetSource];
            }
        } else {    //still some missing packets before buffer can resolve
            sendPacket(connected_gNB->getID(), expectedSeq, PacketType::NACK, std::string("Did not recieve expected Packet, resend Packet#" + std::to_string(expectedSeq)), 1);
        }
        
    }
}
