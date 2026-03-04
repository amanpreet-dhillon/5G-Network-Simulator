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
        void turnOn(const std::vector<gNB*>&);
        void turnOff();
        
        void generateTraffic(int, const std::string&);
        void moveUE(int, int, const std::vector<gNB*>&);
        //void tester_addPacketToReQueue(int, int);

    private:
        
        gNB* connected_gNB;
        bool active;
        bool inNetwork;
        Utils util;
        int receievedPacketSeq; //keep track of order of received packets
        std::unordered_map<int, std::unique_ptr<Packet>> retransmissionQueue;   //stores packets sent by UE temporarily (until ACK is sent by gNB)
        int sentPacketSeq;  //keep track of order of packets sent
        std::map<int, std::unique_ptr<Packet>> buffer;  //stores out-of-order/'future' packets until they can be processsed
        void bufferCleanUp(int);
        void setupRegistrationReq();
        void sendPacket(int, int, PacketType, const std::string&, int); //for ACK, NACK, SKIP
        void sendPacket(int, PacketType, const std::string&, int);  //just for sending DATA


        
};


#endif