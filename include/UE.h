#ifndef UE_H
#define UE_H

#include <vector>
#include <map>
#include "Node.h"
#include "Utils.h"




class gNB;
class Tester;

class UE : public Node {
    public:
        UE(int, int, int);
        ~UE();
        void recievePacket(std::unique_ptr<Packet>);
        void sendPacket(int, int, PacketType, const std::string&, int);
        void turnOn(std::vector<Tester*>);  //testing change
        void turnOff();
        //void sendRegistrationRequest();
        //void sendConfirmationRecieved();

    private:
        Tester* connected_gNB;  //testing change
        bool active;
        Utils util;
        std::map<int, int> recievedPacketTracker; //keep track of packets recieved -- source ID / expected sequence
        std::map<std::pair<int,int>, std::unique_ptr<Packet>> retransmissionQueue;  //stores packets sent to a specific destination  -- destination ID, expected sequence pair / packet sent
        std::map<int, int> destinationSeqTracker; //keep track of the number of packets sent to a specific destination ID (i.e. the sequence number)
        std::map<std::pair<int,int>, std::unique_ptr<Packet>> buffer;
        void bufferCleanUp(int);
        void setupRegistrationReq();

};

#endif