#include "CoreNetwork.h"
#include "gNB.h"
#include <random>
#include <chrono>


CoreNetwork::CoreNetwork(){
    //id = 0;
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
            ueRegistry[ue->getID()] = std::unique_ptr<UE>(static_cast<UE*>(ue.release()));  //release uniqu_ptr, cast from Node* to UE*, create a unique_ptr<UE>
        }

    } else {    //gNBs
        for (auto& gnb : equipment){
            gNBRegistry[gnb->getID()] = std::unique_ptr<gNB>(static_cast<gNB*>(gnb.release())); //release uniqu_ptr, cast from Node* to gNB*, create a unique_ptr<gNB>
        }
    }

}

void CoreNetwork::recievePacket(std::unique_ptr<GTPPacket> packet){

    //unwrap packet

    int destination = ul_TEIDs[packet->TEID];

    if(destination == INTERNET and packet->payload->getPacketType() == PacketType::DATA){   
        //DO SOMETHING
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 2);

        int randomNum = dis(gen);

        if (randomNum == 1){    //send back a packet to Source UE

            auto packetToSend = Packet::createPacket(INTERNET, packet->payload->getSource(), 9999, PacketType::DATA, std::string("Hi this is the Internet!"), 0);
            forwardPacketToUE(packet->payload->getSource(), std::move(packetToSend));

        } else {
            //log packet here
        }


    } else if (destination == CORE_NETWORK){
        
        if (packet->payload->getPacketType() == PacketType::REGISTRATION_REQUEST) {
            
            if(AMF_authenticateUE(packet->payload->getSource(), packet->payload->getData())){   //AMF verfies source UE

                //SMF establishes session with source UE
                activeUEConnections[packet->payload->getSource()] = ueRegistry[packet->payload->getSource()].get();
                
                //UPF creates UL TEID for source UE
                ul_TEIDs[std::stoi(std::to_string(8888) + std::to_string(packet->payload->getSource()))] = packet->payload->getSource();    //create UL TEID for destination UE
                
                //establish which gnb this UE is connected to
                ue_gNb_connection[packet->payload->getSource()] = std::stoi(std::to_string(packet->TEID).substr(0,4));   //first 4 characters of TEID is the gnb ID
                //ue_gNb_connection[dl_TEIDs[packet->payload->getSource()]] = std::stoi(std::to_string(packet->TEID).substr(0,4));

                //AMF sends the connected gnb the UL TEID of this UE
                gNBRegistry[ue_gNb_connection[packet->payload->getSource()]]->recieveUL_TEID(ul_TEIDs[packet->payload->getSource()], packet->payload->getSource()); //send gnb the UL TEID for this UE
                
                
                //include send registration complete here
                auto packetToSend = Packet::createPacket(CORE_NETWORK, packet->payload->getSource(), 0, PacketType::REGISTRATION_COMPLETE, std::string("Core Network has created session for UE #" + std::to_string(packet->payload->getSource())), 1);
                forwardPacketToUE(packetToSend->getDestination(), std::move(packetToSend));

            } else {
                // Log that source could not be registered because it is not valid
            }

           

        }else if(packet->payload->getPacketType() == PacketType::REGISTRATION_ACK){ // registration has been acknowledged by UE
            // Log that UE has sent it has ACK the registration
        }

    } else if (destination >= 1001 and destination <= 1999 and activeUEConnections.count(packet->payload->getDestination())){ //destination is UE and exists in network

        forwardPacketToUE(destination, std::move(packet->payload));
    } 

}

void CoreNetwork::forwardPacketToUE(int destination, std::unique_ptr<Packet> packet){
    //wrap packet
    
    auto wrappedPacket = std::make_unique<GTPPacket>();
    wrappedPacket->TEID = dl_TEIDs[destination];
    wrappedPacket->payload = std::move(packet);
    //still need to implement

    //use DL TEID of destination to find connected gNB
    gNBRegistry[ue_gNb_connection[destination]]->recievePacket(std::move(wrappedPacket));
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