#ifndef TESTER_H
#define TESTER_H

#include "Node.h"
#include "Packet.h"

class UE;

class Tester : public Node {
    public:
        Tester(int, int, int);
        ~Tester();
        void disconnectUE(int);
        void recievePacket(std::unique_ptr<Packet>);
        UE* connectedUE;
        void connectUE(UE*);
        void sendPacket(int, int, PacketType, const std::string&, int);
        
};

#endif