#ifndef UE_H
#define UE_H

#include <vector>
#include <map>
#include <unordered_map>
#include "Node.h"
#include "Utils.h"




class gNB;
class Tester;

class UE : public Node {
    public:
        UE(int, int, int);
        ~UE();
        void recievePacket(std::unique_ptr<Packet>);
        
        void turnOn(std::vector<gNB*>);  //testing change
        void turnOff();
        //void sendRegistrationRequest();
        //void sendConfirmationRecieved();
        void sendPacket(int, int, PacketType, const std::string&, int);

    private:
        
        gNB* connected_gNB;  //testing change
        bool active;
        Utils util;
        
        //std::map<int, int> recievedPacketTracker; //keep track of packets recieved -- source ID / expected sequence
        int receievedPacketSeq; //keep track of order of received packets
        
        //std::map<std::pair<int,int>, std::unique_ptr<Packet>> retransmissionQueue;  //stores packets sent to a specific destination  -- destination ID, expected sequence pair / packet sent
        std::unordered_map<int, std::unique_ptr<Packet>> retransmissionQueue;

        //std::map<int, int> destinationSeqTracker; //keep track of the number of packets sent to a specific destination ID (i.e. the sequence number)
        int sentPacketSeq;

        std::map<int, std::unique_ptr<Packet>> buffer;
        void bufferCleanUp(int);
        void setupRegistrationReq();

};


struct UEContext {
    UE* ue = nullptr;   
    
    int nextDLSeq = 0;
    std::unordered_map<int, std::unique_ptr<Packet>> retransmissionQueue;   //seq, Packet
    
    int expectedULSeq = 1;
    std::map<int, std::unique_ptr<Packet>> buffer;  //seq, Packet
};


#endif