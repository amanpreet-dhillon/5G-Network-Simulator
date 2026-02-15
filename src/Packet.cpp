#include "Packet.h"
#include <format>
#include <sstream>


Packet::Packet(int src, int dest, int seq, PacketType t, const std::string& info, int pri = 0)
            : sourceID {src}, destinationID {dest}, sequenceNumber {seq}, type {t}, data {info}, priority {pri}, timestamp {std::chrono::system_clock::now()}
{

}

Packet::Packet(const Packet& packetToCopy)
            : sourceID {packetToCopy.sourceID}, destinationID {packetToCopy.destinationID}, sequenceNumber {packetToCopy.sequenceNumber}, 
            type {packetToCopy.type}, data {packetToCopy.data}, priority {packetToCopy.priority}, timestamp {packetToCopy.timestamp}
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
    auto trimmedTimestamp = std::chrono::time_point_cast<std::chrono::seconds>(timestamp);
    return std::format("{:%Y-%m-%d %H:%M:%S}", trimmedTimestamp);
}



std::unique_ptr<Packet> Packet::createPacket(int src, int dest, int seq, PacketType t, const std::string& info, int pri){
    return std::make_unique<Packet>(src, dest, seq, t, info, pri);
}

const std::string Packet::print() const{

    std::string packType;
    switch (type)
    {
    case PacketType::REGISTRATION_REQUEST: packType =  "REGISTRATION REQUEST"; break;
    case PacketType::REGISTRATION_COMPLETE: packType =  "REGISTRATION COMPLETE"; break;
    case PacketType::REGISTRATION_ACK: packType =  "REGISTRATION ACKNOWLEDGED"; break;
    case PacketType::DEREGISTRATION_REQUEST: packType =  "DEREGISTRATION REQUEST"; break;
    case PacketType::DATA: packType =  "DATA"; break;
    case PacketType::ACK: packType =  "ACK"; break;
    case PacketType::NACK: packType =  "NACK"; break;
    case PacketType::SKIP: packType =  "SKIP"; break;
     
        
    default: packType =  "UNKNOWN"; break;
    }


    std::string priType;
    switch (priority)
    {
    case -1: priType =  "LOW"; break;
    case 0: priType =  "NEUTRAL"; break;
    case 1: priType =  "HIGH"; break;
        
    default: priType =  "NEUTRAL"; break;
    }


    std::stringstream output;

    output  << "[" << getTimestamp() << "]: "
            << "[ROUTE: " << sourceID << " ---> " << destinationID << "] "
            << "[SEQUENCE: " << sequenceNumber << "] "
            << "[TYPE: " << packType << "] "
            << "[PRIORITY: " << priType << "] "
            << "[PAYLOAD: " << data << "]." << '\n';
    
    return output.str(); 

}

void Packet::overwriteSeq(int newSeq){
    sequenceNumber = newSeq;
}

