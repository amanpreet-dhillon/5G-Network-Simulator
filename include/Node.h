#ifndef NODE_H
#define NODE_H

#include <utility>
#include <memory>
#include "Packet.h"

class Node {

    public:
        Node(int, int, int);
        virtual ~Node();
        int getID(); 
        std::pair<int,int> getLocation() const;
        static const int CORE_NETWORK = 0;
        //float distanceTo(const Node*); //remove?
        //virtual void recievePacket(std::unique_ptr<Packet>); //UNCOMMENT
    
    private:
        int id; //UEs will have ID  1XXX, gNBs will have 5XXX, Internet will be 9999
        int x;
        int y;
        


};

#endif