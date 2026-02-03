#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <chrono>
#include <memory>


enum class PacketType {
    REGISTRATION_REQUEST,
    REGISTRATION_COMPLETE,
    REGISTRATION_ACK,
    DEREGISTRATION_REQUEST,
    MOBILITY,
    DATA,
    ACK,
    NACK,
    SKIP
};

class Packet {

    public:
        Packet(int, int, int, PacketType, const std::string&, int);
        Packet(const Packet&);
        ~Packet();
        int getPriority() const;
        int getSource() const;
        int getDestination() const;
        int getSequenceNum() const;
        PacketType getPacketType() const;
        const std::string getData() const;
        const std::string getTimestamp() const;
        void wrapPacket();
        void unwrapPacket();
        static std::unique_ptr<Packet> createPacket(int, int, int, PacketType, const std::string&, int);
        const std::string print() const;
    
    private:               
        
        int sourceID;
        int destinationID;
        int sequenceNumber;
        PacketType type;
        const std::string data;
        int priority;   // -1 = low priority, 0 = neutral priority, 1 = high priority
        const std::chrono::system_clock::time_point timestamp;

        

};

#endif