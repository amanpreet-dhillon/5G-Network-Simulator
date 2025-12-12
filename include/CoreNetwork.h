#ifndef CORENETWORK_H
#define CORENETWORK_H

#include "Packet.h"
#include <queue>

class CoreNetwork {
    public:
        CoreNetwork();
        ~CoreNetwork();
    
    private:
        std::priority_queue<Packet> packetsFrom_gNB;
        int id;

};

#endif