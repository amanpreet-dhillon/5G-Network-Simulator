#include "Packet.h"
#include <format>
#include <sstream>


Packet::Packet(int src, int dest, int seq, PacketType t, const std::string& info, int pri = 0)
            : sourceID {src}, destinationID {dest}, sequenceNumber {seq}, type {t}, data {info}, priority {pri}, timestamp {std::chrono::system_clock::now()}
{

}

Packet::~Packet(){

}

int Packet::getPriority() const {
    return priority;
}

int Packet::getSource() const {
    return sourceID;
}

int Packet::getDestination() const {
    return destinationID;
}

const std::string Packet::getData() const{
    return data;
}

int Packet::getSequenceNum() const {
    return sequenceNumber;
}

PacketType Packet::getPacketType() const {
    return type;
}

const std::string Packet::getTimestamp() const {
     return std::format("{:%Y-%m-%d %H:%M:%S}", timestamp);
}



std::unique_ptr<Packet> Packet::createPacket(int src, int dest, int seq, PacketType t, const std::string& info, int pri){
    return std::make_unique<Packet>(src, dest, seq, t, info, pri);
}

const std::string Packet::print() const{

    std::string packType;
    switch (type)
    {
    case PacketType::REGISTRATION_REQUEST: packType =  "REGISTRATION REQUEST";
    case PacketType::REGISTRATION_COMPLETE: packType =  "REGISTRATION COMPLETE";
    case PacketType::REGISTRATION_ACK: packType =  "REGISTRATION ACKNOWLEDGED";
    case PacketType::DEREGISTRATION_REQUEST: packType =  "DEREGISTRATION REQUEST";
    case PacketType::DATA_UPLINK: packType =  "DATA UPLINK";
    case PacketType::DATA_DOWNLINK: packType =  "DATA DOWNLINK";
     
        
    default: packType =  "UNKNOWN";
    }


    std::string priType;
    switch (priority)
    {
    case -1: priType =  "LOW";
    case 0: priType =  "NEUTRAL";
    case 1: priType =  "HIGH";
        
    default: priType =  "NEUTRAL";
    }


    std::stringstream output;

    output  << "[" << getTimestamp() << "]; "
            << "[ROUTE: " << sourceID << " ---> " << destinationID << "]; "
            << "[SEQUENCE: " << sequenceNumber << "]; "
            << "[TYPE: " << packType << "]; "
            << "[PRIORITY: " << priType << "].";
    
    return output.str(); 

}

