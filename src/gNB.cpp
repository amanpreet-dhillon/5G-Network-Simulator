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
    auto ueContext = std::make_unique<UEContext>();
    ueContext->ue = ue;
    ueRegistery[ue->getID()] = std::move(ueContext);
    
}

void gNB::disconnectUE(UE* ue){
       
    if (ueRegistery.count(ue->getID())){
        //coreNetwork->removeUE(ue->getID()); ????
        //ueRegistery[ue->getID()]->ue = nullptr;
        ueRegistery.erase(ue->getID());
    }
}

void gNB::recievePacket(std::unique_ptr<Packet> recievedPacket){   //this will come from a UE, if DATA or registration related then wrap and forward to Core Network, else other handles (UL)
    auto sourceUE = ueRegistery.find(recievedPacket->getSource());

    //ACK -> recievedPacket source is the source UE of original packet, destination is the destination UE of original packet
    //NACK ->  recievedPacket source is the source UE of original packet, destination is the destination UE of original packet

    if(sourceUE != ueRegistery.end()){

        auto& retransmissionQueue = sourceUE->second->retransmissionQueue;

        if (recievedPacket->getPacketType() == PacketType::ACK){

            if(retransmissionQueue.find(recievedPacket->getSequenceNum()) != retransmissionQueue.end()){    //check if the packet exists in retransmissionQueue
                retransmissionQueue.erase(recievedPacket->getSequenceNum());
            }

        } else if (recievedPacket->getPacketType() == PacketType::NACK) {   //For NACK, UE will switch source and destination s.t. the source is now source of original packet (not the currently sending UE) and destination is the original recieving UE (the one sending the NACK)

            if (retransmissionQueue.find(recievedPacket->getSequenceNum()) != retransmissionQueue.end()){
                std::unique_ptr<Packet> packetToSend = std::make_unique<Packet>(*retransmissionQueue[recievedPacket->getSequenceNum()]); //copy via copy constructor
                //connectedUEs[recievedPacket->getSource()]->recievePacket(std::move(packetToSend));
                sourceUE->second->ue->recievePacket(std::move(packetToSend));
            } else {
                sendPacket(recievedPacket->getSource(), recievedPacket->getDestination(), -1, PacketType::SKIP, std::string("Could not find Packet #" + std::to_string(recievedPacket->getSequenceNum()) + "from source #" + std::to_string(recievedPacket->getDestination())), 1);
            }

        } else if (recievedPacket->getPacketType() == PacketType::SKIP) {

            sourceUE->second->expectedULSeq++;
            bufferCleanUp(recievedPacket->getSource());
            
        } else if (recievedPacket->getPacketType() == PacketType::DATA){

            int& expectedSeq = sourceUE->second->expectedULSeq;
            // //verify if it is the epxected sequence number
            // if (recievedPacketTracker.find({recievedPacket->getSource(), recievedPacket->getDestination()}) != recievedPacketTracker.end()) {
            //     expectedSeq = recievedPacketTracker[{recievedPacket->getSource(), recievedPacket->getDestination()}];
            // } else {
            //     recievedPacketTracker[{recievedPacket->getSource(), recievedPacket->getDestination()}] = 1;
            //     expectedSeq = recievedPacketTracker[{recievedPacket->getSource(), recievedPacket->getDestination()}];
            // }

            if (recievedPacket->getSequenceNum() == expectedSeq){   //got expected packet, send ACK, update the seq for that source
                
                sendPacket(recievedPacket->getSource(), recievedPacket->getDestination(), recievedPacket->getSequenceNum(), PacketType::ACK, std::string("ACK packet# " + std::to_string(recievedPacket->getSequenceNum()) + " from " + std::to_string(recievedPacket->getSource()) + " to " +  std::to_string(recievedPacket->getSource())), 1);
                //include logger here
                sourceUE->second->expectedULSeq++;   //increment expected counter
                bufferCleanUp(recievedPacket->getSource());
                
                forwardToCore(std::move(recievedPacket));
            
            } else if (recievedPacket->getSequenceNum() < expectedSeq){    //recieved an old/duplicate packet, send ACK only
            
                sendPacket(recievedPacket->getSource(), recievedPacket->getDestination(), 0, PacketType::ACK, std::string("Duplicate packet recieved, packet# " + std::to_string(recievedPacket->getSequenceNum()) + " from " + std::to_string(recievedPacket->getSource()) + " to " +  std::to_string(recievedPacket->getSource())), 1); //send ACk for old/dupe packet recieved
            
            } else if (recievedPacket->getSequenceNum() > expectedSeq){     //recieved a future/out-of-order packet, store it and ask for missing packets
                
                auto& buffer = sourceUE->second->buffer;
                
                buffer[recievedPacket->getSequenceNum()] = std::move(recievedPacket);    //move 'future' packet to buffer
                sendPacket(recievedPacket->getSource(), recievedPacket->getDestination(), expectedSeq, PacketType::NACK, std::string("Did not recieve expected Packet, resend Packet #" + std::to_string(expectedSeq) + " from " + std::to_string(recievedPacket->getSource()) + " to " + std::to_string(recievedPacket->getDestination())), 1);     //ping gNB to send expectedSeq packet, using source of packet as destination

            }

        } else {    //Registration related

            forwardToCore(std::move(recievedPacket));
            
        }
    }
    



    
}

void gNB::recievePacket(std::unique_ptr<GTPPacket> data){    //this will come from Core Netwwork, unwrap, send to destination UE (DL)

    // // inlcude retransmissionQueue logic
    // //int seq {++destinationSeqTracker[{data->payload->getSource(), data->payload->getDestination()}]};   //update sequence number of destination
    // std::unique_ptr<Packet> packetToStore = Packet::createPacket(data->payload->getSource(), data->payload->getDestination(), data->payload->getSequenceNum(), data->payload->getPacketType(), data->payload->getData(), data->payload->getPriority());    //packet for storage
    // //std::cout << packetToSend->print();
    // retransmissionQueue[{{data->payload->getSource(), data->payload->getDestination()}, seq}] = std::move(packetToStore);   
    
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

void gNB::establishConnectionToCore(const CoreNetwork* cN){
    coreNetwork = cN;
}

void gNB::sendPacket(int sourceID, int destID, int seq, PacketType pType, const std::string& data, int prio=0){   //used for ACK/NACK/SKIP related packets


    //ACK -> send {dest, seq} as dest, seq of ACK packet
    //SKIP -> send source of packet to UE as the source to skip from
    //NACK -> send dest ID as the destination of the missing packet needed from source UE
    if (ueRegistery.count(sourceID)){
        if (pType == PacketType::ACK){
            std::unique_ptr<Packet> packetToSend = Packet::createPacket(sourceID, destID, seq, pType, data, prio);
            ueRegistery[sourceID]->ue->recievePacket(std::move(packetToSend));
        } else if (pType == PacketType::NACK){
            std::unique_ptr<Packet> packetToSend = Packet::createPacket(sourceID, destID, seq, pType, data, prio);
            ueRegistery[sourceID]->ue->recievePacket(std::move(packetToSend));
        } else if (pType == PacketType::SKIP){
            std::unique_ptr<Packet> packetToSend = Packet::createPacket(sourceID, destID, seq, pType, data, prio);
            ueRegistery[sourceID]->ue->recievePacket(std::move(packetToSend));
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
                buffer.erase(receievedPacketSeq); //remove acknowledged packet from buffer
                receievedPacketSeq++; 
            }
        } else {    //still some missing packets before buffer can resolve
            sendPacket(this->getID(), sourceID, receievedPacketSeq, PacketType::NACK, std::string("Did not recieve expected Packet, resend Packet #" + std::to_string(receievedPacketSeq)), 1);
        }
    
    }
}