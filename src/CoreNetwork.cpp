#include "CoreNetwork.h"
#include "gNB.h"
#include <random>
#include <chrono>
#include <iostream>
#include "Utils.h"
#include "Logger.h"

CoreNetwork::CoreNetwork(){
    ul_TEIDs[CORE_NETWORK] = CORE_NETWORK;
    ul_TEIDs[INTERNET] = INTERNET;
}

CoreNetwork::~CoreNetwork(){
    activeUEConnections.clear();
    ueRegistry.clear();
    gNBRegistry.clear();

}

void CoreNetwork::recieveDL_TEID(int dl_TEID, int ue_id){
    dl_TEIDs[ue_id] = dl_TEID;
}

void CoreNetwork::loadEquipment(std::vector<std::unique_ptr<Node>> equipment, const std::string& type){

    if (type.compare("UE") == 0){   //UEs

        for (auto& ue : equipment){
            int ueID = ue->getID();
            ueRegistry[ueID] = std::unique_ptr<UE>(static_cast<UE*>(ue.release()));  //release uniqu_ptr, cast from Node* to UE*, create a unique_ptr<UE>
        }

    } else {    //gNBs
        for (auto& gnb : equipment){
            int gnbID = gnb->getID();
            gNBRegistry[gnbID] = std::unique_ptr<gNB>(static_cast<gNB*>(gnb.release())); //release uniqu_ptr, cast from Node* to gNB*, create a unique_ptr<gNB>
        }
    }

}

void CoreNetwork::removeUE(int ueID) {
    activeUEConnections[ueID] = nullptr;
    activeUEConnections.erase(ueID);

    ue_gNb_connection.erase(ueID);
    dl_TEIDs.erase(ueID);

    for (auto pair = ul_TEIDs.begin(); pair != ul_TEIDs.end();){
        if (pair->second == ueID){
            pair = ul_TEIDs.erase(pair);
        } else {
            ++pair;
        }
    }


}


void CoreNetwork::recievePacket(std::unique_ptr<GTPPacket> packet){

    //unwrap packet
    int destination {packet->payload->getDestination()};

    if(destination == INTERNET and packet->payload->getPacketType() == PacketType::DATA){   //Packet should be data related
        
        Logger::getInstance().logPacketRecieved(9999, packet->payload->print());

        int randomNum = Utils::generateRandNum(1, 2);
        internet.recieveData(packet->payload->getData());

        if (randomNum == 1){    //send back a packet to Source UE

            auto packetToSend = Packet::createPacket(INTERNET, packet->payload->getSource(), 9999, PacketType::DATA, internet.getRandomResponse(), 0);
            forwardPacketToUE(packet->payload->getSource(), std::move(packetToSend));

        }


    } else if (destination == CORE_NETWORK){    //Packet should be Registration related or Mobility
        
        Logger::getInstance().logPacketRecieved(0, packet->payload->print());

        if (packet->payload->getPacketType() == PacketType::REGISTRATION_REQUEST) {
            
            if(AMF_authenticateUE(packet->payload->getSource(), packet->payload->getData())){   //AMF verfies source UE

                std::string logInfo = " --- UE " + std::to_string(packet->payload->getSource()) + " has been authenticated.";
                Logger::getInstance().logOther(0, logInfo);

                //SMF establishes session with source UE
                activeUEConnections[packet->payload->getSource()] = ueRegistry[packet->payload->getSource()].get();
                logInfo = " --- SMF has established session for UE " + std::to_string(packet->payload->getSource());
                Logger::getInstance().logOther(0, logInfo);
                
                //UPF creates UL TEID for source UE
                ul_TEIDs[std::stoi(std::to_string(8888) + std::to_string(packet->payload->getSource()))] = packet->payload->getSource();    //create UL TEID for destination UE
                logInfo = " --- UPF has created UL TEID for UE " + std::to_string(packet->payload->getSource());
                Logger::getInstance().logOther(0, logInfo);
                
                //establish which gnb this UE is connected to
                ue_gNb_connection[packet->payload->getSource()] = std::stoi(std::to_string(packet->TEID).substr(0,4));   //first 4 characters of TEID is the gnb ID

                //AMF sends the connected gnb the UL TEID of this UE
                gNBRegistry[ue_gNb_connection[packet->payload->getSource()]]->recieveUL_TEID(std::stoi(std::to_string(8888) + std::to_string(packet->payload->getSource())), packet->payload->getSource()); //send gnb the UL TEID for this UE
                logInfo = " --- AMF has sent gNB the UL TEID for UE " + std::to_string(packet->payload->getSource());
                Logger::getInstance().logOther(0, logInfo);
                
                //include send registration complete here
                auto packetToSend = Packet::createPacket(CORE_NETWORK, packet->payload->getSource(), 0, PacketType::REGISTRATION_COMPLETE, std::string("Core Network has created session for UE #" + std::to_string(packet->payload->getSource()) + " and connected to gNB #" + std::to_string(ue_gNb_connection[packet->payload->getSource()])), 1);
                forwardPacketToUE(packet->payload->getSource(), std::move(packetToSend));

            } else {
                std::string logInfo = "This equipment could not be registered";
                Logger::getInstance().logOther(0, logInfo);
            }

           

        }else if(packet->payload->getPacketType() == PacketType::REGISTRATION_ACK){ // registration has been acknowledged by UE
            // Log that UE has sent it has ACK the registration
            std::string logInfo = " --- UE " + std::to_string(packet->payload->getSource()) + " has acknowledged registration.";
            Logger::getInstance().logOther(0, logInfo);

        } else if (packet->payload->getPacketType() == PacketType::DEREGISTRATION_REQUEST){
            std::string logInfo = " --- UE " + std::to_string(packet->payload->getSource()) + " has requested to be unregistered.";
            Logger::getInstance().logOther(0, logInfo);
            
            removeUE(packet->payload->getSource());
            
        } else if(packet->payload->getPacketType() == PacketType::MOBILITY){
            auto new_gNBID = std::stoi(split(packet->payload->getData(), ',')[3]);
            moveUE(packet->payload->getSource(), new_gNBID);

            std::string logInfo = " UE " + std::to_string(packet->payload->getSource()) + " has moved, now connected to gNB " + std::to_string(new_gNBID);
            Logger::getInstance().logOther(0, logInfo);
        }

    } else if (destination >= 1001 and destination <= 1999){ //destination is UE and exists in network
        std::string logInfo = " has recieved a packet from " + std::to_string(packet->payload->getSource()) + " to " + std::to_string(packet->payload->getSource()) + ". Beginning handover process.\n";
        Logger::getInstance().logOther(0, logInfo);

        forwardPacketToUE(destination, std::move(packet->payload));
    } 

}

void CoreNetwork::forwardPacketToUE(int destination, std::unique_ptr<Packet> packet){

    if (activeUEConnections.count(destination)){    //destination UE must be actively connected to network
        //wrap packet
        auto wrappedPacket = std::make_unique<GTPPacket>();
        wrappedPacket->TEID = dl_TEIDs[destination];
        wrappedPacket->payload = std::move(packet);

        //use DL TEID of destination to find connected gNB
        Logger::getInstance().logPacketSent(0, wrappedPacket->payload->print());
        gNBRegistry[ue_gNb_connection[destination]]->recievePacket(std::move(wrappedPacket));    
    }
    
}

bool CoreNetwork::AMF_authenticateUE(int ueID, const std::string& data){    //verify if UE exists in ueRegistry and its location
    std::vector<std::string> splitData = split(data, ',');

    if(ueRegistry.count(ueID)){
        if(std::stoi(splitData[1]) == ueRegistry[ueID]->getLocation().first and std::stoi(splitData[2]) == ueRegistry[ueID]->getLocation().second){
            return true;
        }
    }

    return false;
}

std::vector<std::string> CoreNetwork::split(const std::string& str, char delimiter){    //helper function to split a string at a delimiter
    std::vector<std::string> result;
    std::string substr;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, substr, delimiter)){
        result.push_back(substr);
    }

    return result;
}

void CoreNetwork::moveUE(int ueID, int new_gNBID){
    
    gNBRegistry[ue_gNb_connection[ueID]]->disconnectUE(ueID);    //disconnect UE from old gnb

    gNBRegistry[new_gNBID]->connectUE(ueRegistry[ueID].get());
    gNBRegistry[new_gNBID]->recieveUL_TEID(std::stoi(std::to_string(8888) + std::to_string(ueID)), ueID);

    ue_gNb_connection[ueID] = new_gNBID;
    
}