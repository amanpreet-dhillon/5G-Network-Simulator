#ifndef NODE_H
#define NODE_H

#include <utility>
#include "Packet.h"

class Node {

    public:
        Node(int, int, int);
        virtual ~Node();
        int getID(); 
        std::pair<int,int> getLocation() const;
        //float distanceTo(const Node*);
        virtual void recievePacket(const Packet&);
    
    private:
        int id;
        int x;
        int y;
        


};

#endif