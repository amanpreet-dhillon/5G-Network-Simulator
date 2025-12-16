#ifndef GNB_H
#define GNB_H

#include "Node.h"
#include "CoreNetwork.h" 
#include "Packet.h"
#include <vector>
#include <queue>

class UE;

class gNB : public Node {
    public:
        gNB();
        ~gNB();
        void addUE();
        void removeUE();
        void recievePacket(std::unique_ptr<Packet>);
        void forwardToCore(const CoreNetwork&);
        //void disconnectUE(int);   /remove?


    private:
        std::vector<UE*> connectedUEs;
        std::priority_queue<Packet> packets;


};

#endif