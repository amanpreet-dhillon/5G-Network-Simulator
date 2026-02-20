#include "gNB.h"
#include "UE.h"
#include <iostream>
#include <algorithm>
#include <string>

gNB::gNB(int givenID, int xcord, int ycord) : Node(givenID, xcord, ycord){
    ul_TEIDs[0] = 0;    //CORE NETWORK
    ul_TEIDs[9999] = 9999;  //INTERNET
}

gNB::~gNB(){

    ueRegistery.clear();

}

void gNB::connectUE(UE* ue){

    auto dlTEID = std::stoi(std::to_string(this->getID()) + std::to_string(ue->getID()));
    
    //int dlTEID = 40000 + ue->getID();
    dl_TEIDs[dlTEID] = ue->getID();
    //coreNetwork->recieveDL_TEID(dlTEID, ue->getID());
    auto ueContext = std::make_unique<UEContext>();
    ueContext->ue = ue;
    ueRegistery[ue->getID()] = std::move(ueContext);

    std::cout << "gNB has added UE#" + std::to_string(ueRegistery[ue->getID()]->ue->getID()) << std::endl;
    
}

void gNB::disconnectUE(UE* ue){
       
    if (ueRegistery.count(ue->getID())){
        //coreNetwork->removeUE(ue->getID()); ????
        ueRegistery[ue->getID()]->ue = nullptr;
        ueRegistery.erase(ue->getID());
    }
}

void gNB::recievePacket(std::unique_ptr<Packet> recievedPacket){   //this will come from a UE, if DATA or registration related then wrap and forward to Core Network, else other handles (UL)
    auto sourceUE = ueRegistery.find(recievedPacket->getSource());

    if(sourceUE != ueRegistery.end()){

        std::cout  << recievedPacket->print() << std::endl;

        auto& retransmissionQueue = sourceUE->second->retransmissionQueue;

        if (recievedPacket->getPacketType() == PacketType::ACK){

            if(retransmissionQueue.find(recievedPacket->getSequenceNum()) != retransmissionQueue.end()){    //check if the packet exists in retransmissionQueue
                retransmissionQueue.erase(recievedPacket->getSequenceNum());
            }

        } else if (recievedPacket->getPacketType() == PacketType::NACK) {   //For NACK, UE will switch source and destination s.t. the source is now source of original packet (not the currently sending UE) and destination is the original recieving UE (the one sending the NACK)

            if (retransmissionQueue.find(recievedPacket->getSequenceNum()) != retransmissionQueue.end()){

                std::unique_ptr<Packet> packetToSend = std::make_unique<Packet>(*retransmissionQueue[recievedPacket->getSequenceNum()]); //copy via copy constructor
                sourceUE->second->ue->recievePacket(std::move(packetToSend));

            } else {

                sendPacket(this->getID(), recievedPacket->getSource(), recievedPacket->getSequenceNum(), PacketType::SKIP, std::string("Could not find Packet #" + std::to_string(recievedPacket->getSequenceNum()) + " from source #" + std::to_string(recievedPacket->getDestination())), 1);
                sourceUE->second->nextDLSeq = std::max(sourceUE->second->nextDLSeq + 1, recievedPacket->getSequenceNum() + 1);

            }

        } else if (recievedPacket->getPacketType() == PacketType::SKIP) {

            sourceUE->second->expectedULSeq = std::max(sourceUE->second->expectedULSeq + 1, recievedPacket->getSequenceNum() + 1);
            bufferCleanUp(recievedPacket->getSource());
            
        } else if (recievedPacket->getPacketType() == PacketType::DATA){

            int& expectedSeq = sourceUE->second->expectedULSeq;
            
            if (recievedPacket->getSequenceNum() == expectedSeq){   //got expected packet, send ACK, update the seq for that source
                
                sendPacket(this->getID(), recievedPacket->getSource(), recievedPacket->getSequenceNum(), PacketType::ACK, std::string("ACK packet# " + std::to_string(recievedPacket->getSequenceNum()) + " from " + std::to_string(recievedPacket->getSource()) + " to " +  std::to_string(recievedPacket->getDestination())), 1);
                //include logger here
                sourceUE->second->expectedULSeq++;   //increment expected counter
                bufferCleanUp(recievedPacket->getSource());
                
                forwardToCore(std::move(recievedPacket));
            
            } else if (recievedPacket->getSequenceNum() < expectedSeq){    //recieved an old/duplicate packet, send ACK only
            
                sendPacket(this->getID(), recievedPacket->getSource(), 0, PacketType::ACK, std::string("Old packet recieved, packet# " + std::to_string(recievedPacket->getSequenceNum()) + " from " + std::to_string(recievedPacket->getSource()) + " to " +  std::to_string(recievedPacket->getDestination())), 1); //send ACk for old/dupe packet recieved
            
            } else if (recievedPacket->getSequenceNum() > expectedSeq){     //recieved a future/out-of-order packet, store it and ask for missing packets
                
                int source {recievedPacket->getSource()};
                int dest {recievedPacket->getDestination()};

                auto& buffer = sourceUE->second->buffer;
                buffer[recievedPacket->getSequenceNum()] = std::move(recievedPacket);    //move 'future' packet to buffer                
                
                sendPacket(this->getID(), source, expectedSeq, PacketType::NACK, std::string("Did not recieve expected Packet, resend Packet #" + std::to_string(expectedSeq) + " from " + std::to_string(source) + " to " + std::to_string(dest)), 1);     //ping gNB to send expectedSeq packet, using source of packet as destination
            
            }

        } else {    //Registration related

            forwardToCore(std::move(recievedPacket));
            
        }
    }
    



    
}

void gNB::recievePacket(std::unique_ptr<GTPPacket> data){    //this will come from Core Netwwork, unwrap, send to destination UE (DL)    
    
    //unwrapping packet
    int destinationUE = dl_TEIDs[data->TEID];   
    sendPacket(destinationUE, std::move(data->payload));   
}

void gNB::forwardToCore(std::unique_ptr<Packet> packetToForward){ 
    
    //wrapping packet
    auto wrappedPacket = std::make_unique<GTPPacket>();
    wrappedPacket->TEID = ul_TEIDs[packetToForward->getDestination()]; 
    wrappedPacket->payload = std::move(packetToForward);    
    //coreNetwork->recievePacket(std::move(wrappedPacket))
}

void gNB::establishConnectionToCore(CoreNetwork* cN){
    coreNetwork = cN;
}

void gNB::sendPacket(int sourceID, int destID, int seq, PacketType pType, const std::string& data, int prio=0){   //used for ACK/NACK/SKIP related packets

    if (ueRegistery.count(destID)){
        if (pType == PacketType::ACK){
            std::unique_ptr<Packet> packetToSend = Packet::createPacket(this->getID(), destID, seq, pType, data, prio);
            ueRegistery[destID]->ue->recievePacket(std::move(packetToSend));
        } else if (pType == PacketType::NACK){
            std::unique_ptr<Packet> packetToSend = Packet::createPacket(this->getID(), destID, seq, pType, data, prio);
            ueRegistery[destID]->ue->recievePacket(std::move(packetToSend));
        } else if (pType == PacketType::SKIP){
            std::unique_ptr<Packet> packetToSend = Packet::createPacket(this->getID(), destID, seq, pType, data, prio);
            ueRegistery[destID]->ue->recievePacket(std::move(packetToSend));
        }
    }
  
}

void gNB::sendPacket(int destID, std::unique_ptr<Packet> packet){ //used for sending DATA or registration related packets
    if(ueRegistery.count(destID)){

        if(packet->getPacketType() == PacketType::DATA){
            
            //add to retransmissionQueue
            ueRegistery[destID]->nextDLSeq++;
            packet->overwriteSeq(ueRegistery[destID]->nextDLSeq);
            std::unique_ptr<Packet> packetToStore = std::make_unique<Packet>(*packet);
            ueRegistery[destID]->retransmissionQueue[ueRegistery[destID]->nextDLSeq] = std::move(packetToStore);   
            
            //send packet to UE
            ueRegistery[destID]->ue->recievePacket(std::move(packet));

        } else {    //for registration reltaed packets
            ueRegistery[destID]->ue->recievePacket(std::move(packet));
        }

      
    }
}

void gNB::recieveUL_TEID(int ul_TEID, int ueID){
    ul_TEIDs[ueID] = ul_TEID; 
}


void gNB::bufferCleanUp(int sourceID){

    auto& buffer = ueRegistery[sourceID]->buffer;
    auto& receievedPacketSeq = ueRegistery[sourceID]->expectedULSeq;

    if(!buffer.empty()){   //check if recieved packet is in buffer
                
        if(buffer.find(receievedPacketSeq) != buffer.end()){

            while(buffer.find(receievedPacketSeq) != buffer.end()){

                sendPacket(this->getID(), sourceID, receievedPacketSeq, PacketType::ACK, std::string("Recieved Packet #" + std::to_string(receievedPacketSeq) + " from source #" + std::to_string(buffer[receievedPacketSeq]->getSource())), 1); //send ACk for packet recieved
                forwardToCore(std::move(buffer[receievedPacketSeq]));
                buffer.erase(receievedPacketSeq); //remove acknowledged packet from buffer
                receievedPacketSeq++; 

            }
        } else {    //still some missing packets before buffer can resolve
            sendPacket(this->getID(), sourceID, receievedPacketSeq, PacketType::NACK, std::string("Did not recieve expected Packet, resend Packet #" + std::to_string(receievedPacketSeq)), 1);
        }
    
    }
}

void gNB::tester_addPacketToReQueue(int ue_id, int seq){

    if(ueRegistery.count(ue_id)){

        auto& ueContext = ueRegistery[ue_id];
        ueContext->retransmissionQueue[seq] = std::move(std::make_unique<Packet>(1005, 1001, seq, PacketType::DATA, "queued packet #" + std::to_string(seq), 0));

    }
}