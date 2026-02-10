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
    connectedUEs[ue->getID()] = ue;
       
}

void gNB::disconnectUE(UE* ue){
       
    if (connectedUEs.count(ue->getID())){
        //coreNetwork->removeUE(connectedUEs[UElocation]); ????
        connectedUEs.erase(ue->getID());
    }
}

void gNB::recievePacket(std::unique_ptr<Packet> recievedPacket){   //this will come from a UE, if DATA or registration related then wrap and forward to Core Network, else other handles (UL)


    //ACK -> recievedPacket source is the source UE of original packet, destination is the destination UE of original packet
    //NACK ->  recievedPacket source is the source UE of original packet, destination is the destination UE of original packet

    if (recievedPacket->getPacketType() == PacketType::ACK){

        if(retransmissionQueue.find({{recievedPacket->getSource(), recievedPacket->getDestination()}, recievedPacket->getSequenceNum()}) != retransmissionQueue.end()){    //check if the packet exists in retransmissionQueue
            retransmissionQueue.erase({{recievedPacket->getSource(), recievedPacket->getDestination()}, recievedPacket->getSequenceNum()});
        }

    } else if (recievedPacket->getPacketType() == PacketType::NACK) {   //For NACK, UE will switch source and destination s.t. the source is now source of original packet (not the currently sending UE) and destination is the original recieving UE (the one sending the NACK)

        if (retransmissionQueue.find({{recievedPacket->getSource(), recievedPacket->getDestination()}, recievedPacket->getSequenceNum()}) != retransmissionQueue.end()){
            std::unique_ptr<Packet> packetToSend = std::make_unique<Packet>(*retransmissionQueue[{{recievedPacket->getSource(), recievedPacket->getDestination()}, recievedPacket->getSequenceNum()}]); //copy via copy constructor
            connectedUEs[recievedPacket->getSource()]->recievePacket(std::move(packetToSend));
        } else {
            sendPacket(recievedPacket->getSource(), recievedPacket->getDestination(), -1, PacketType::SKIP, std::string("Could not find Packet #" + std::to_string(recievedPacket->getSequenceNum()) + "from source #" + std::to_string(recievedPacket->getDestination())), 1);
        }

    } else if (recievedPacket->getPacketType() == PacketType::SKIP) {

        recievedPacketTracker[{recievedPacket->getSource(), recievedPacket->getDestination()}]++;
        bufferCleanUp(recievedPacket->getSource(), recievedPacket->getDestination());
        
    } else if (recievedPacket->getPacketType() == PacketType::DATA){

        int expectedSeq;
        //verify if it is the epxected sequence number
        if (recievedPacketTracker.find({recievedPacket->getSource(), recievedPacket->getDestination()}) != recievedPacketTracker.end()) {
            expectedSeq = recievedPacketTracker[{recievedPacket->getSource(), recievedPacket->getDestination()}];
        } else {
            recievedPacketTracker[{recievedPacket->getSource(), recievedPacket->getDestination()}] = 1;
            expectedSeq = recievedPacketTracker[{recievedPacket->getSource(), recievedPacket->getDestination()}];
        }

        if (recievedPacket->getSequenceNum() == expectedSeq){   //got expected packet, send ACK, update the seq for that source
            
            sendPacket(recievedPacket->getSource(), recievedPacket->getDestination(), recievedPacket->getSequenceNum(), PacketType::ACK, std::string("ACK packet# " + std::to_string(recievedPacket->getSequenceNum()) + " from " + std::to_string(recievedPacket->getSource()) + " to " +  std::to_string(recievedPacket->getSource())), 1);
            //include logger here
            recievedPacketTracker[{recievedPacket->getSource(), recievedPacket->getDestination()}]++;   //increment expected counter
            expectedSeq = recievedPacketTracker[{recievedPacket->getSource(), recievedPacket->getDestination()}];

            bufferCleanUp(recievedPacket->getSource(), recievedPacket->getDestination());
            
            forwardToCore(std::move(recievedPacket));
        
        } else if (recievedPacket->getSequenceNum() < expectedSeq){    //recieved an old/duplicate packet, send ACK only
        
            sendPacket(recievedPacket->getSource(), recievedPacket->getDestination(), 0, PacketType::ACK, std::string("Duplicate packet recieved, packet# " + std::to_string(recievedPacket->getSequenceNum()) + " from " + std::to_string(recievedPacket->getSource()) + " to " +  std::to_string(recievedPacket->getSource())), 1); //send ACk for old/dupe packet recieved
        
        } else if (recievedPacket->getSequenceNum() > expectedSeq){     //recieved a future/out-of-order packet, store it and ask for missing packets
            
            buffer[{recievedPacket->getSource(), recievedPacket->getSequenceNum()}] = std::move(recievedPacket);    //move 'future' packet to buffer
            sendPacket(recievedPacket->getSource(), recievedPacket->getDestination(), expectedSeq, PacketType::NACK, std::string("Did not recieve expected Packet, resend Packet #" + std::to_string(expectedSeq) + " from " + std::to_string(recievedPacket->getSource()) + " to " + std::to_string(recievedPacket->getDestination())), 1);     //ping gNB to send expectedSeq packet, using source of packet as destination

        }

    } else {    //Registration related

        forwardToCore(std::move(recievedPacket));
        
    }



    
}

void gNB::recievePacket(std::unique_ptr<GTPPacket> data){    //this will come from Core Netwwork, store in retransmissionQueue, unwrap, send to destination UE (DL)

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

    if (pType == PacketType::ACK){
        std::unique_ptr<Packet> packetToSend = Packet::createPacket(sourceID, destID, seq, pType, data, prio);
        connectedUEs[sourceID]->recievePacket(std::move(packetToSend));
    } else if (pType == PacketType::NACK){
        std::unique_ptr<Packet> packetToSend = Packet::createPacket(sourceID, destID, seq, pType, data, prio);
        connectedUEs[sourceID]->recievePacket(std::move(packetToSend));
    } else if (pType == PacketType::SKIP){
        std::unique_ptr<Packet> packetToSend = Packet::createPacket(sourceID, destID, seq, pType, data, prio);
        connectedUEs[sourceID]->recievePacket(std::move(packetToSend));
    }
}

void gNB::sendPacket(int destID, std::unique_ptr<Packet> packet){ //used for sending DATA or registration related packets
    if(connectedUEs.count(destID)){

        //add to retransmissionQueue
        int seq {++destinationSeqTracker[{packet->getSource(), packet->getDestination()}]};   //update sequence number of destination
        std::unique_ptr<Packet> updatedPacket = Packet::createPacket(packet->getSource(), packet->getDestination(), seq, packet->getPacketType(), packet->getData(), packet->getPriority());    //packet for storage
        std::unique_ptr<Packet> packetToStore = std::make_unique<Packet>(*updatedPacket);
        retransmissionQueue[{{packet->getSource(), packet->getDestination()}, packet->getSequenceNum()}] = std::move(packetToStore);   
        
        //send packet to UE
        connectedUEs[destID]->recievePacket(std::move(updatedPacket));
    }
}

void gNB::recieveUL_TEID(int ul_TEID, int ueID){
    ul_TEIDs[ueID] = ul_TEID; 
}


void gNB::bufferCleanUp(int packetSource, int packetDestination){

    int expectedSeq {recievedPacketTracker[{packetSource, packetDestination}]};

    if(!buffer.empty()){   //check if recieved packet is in buffer
                    
        if(buffer.find({packetSource, expectedSeq}) != buffer.end()){
            while(buffer.find({packetSource, expectedSeq}) != buffer.end()){
                sendPacket(packetSource, packetDestination, expectedSeq, PacketType::ACK, std::string("Recieved Packet #" + std::to_string(expectedSeq)), 1); //send ACk for packet recieved
                buffer.erase({packetSource, expectedSeq}); //remove acknowledged packet from buffer
                recievedPacketTracker[{packetSource, packetDestination}]++; 
                expectedSeq = recievedPacketTracker[{packetSource, packetDestination}];
            }
        } else {    //still some missing packets before buffer can resolve
            sendPacket(packetSource, packetDestination, expectedSeq, PacketType::NACK, std::string("Did not recieve expected Packet, resend Packet #" + std::to_string(expectedSeq)), 1);
        }
        
    }
}