#ifndef GNB_H
#define GNB_H

#include "Node.h"
#include "CoreNetwork.h" 
#include <Packet.h>
#include <vector>
#include <queue>

class UE;

class gNB : public Node {
    public:
        gNB();
        ~gNB();
        void addUE();
        void removeUE();
        void recievePacket(const Packet&);
        void forwardToCore(const CoreNetwork&);


    private:
        std::vector<UE*> connectedUEs;
        std::priority_queue<Packet> packets;


};

#endif