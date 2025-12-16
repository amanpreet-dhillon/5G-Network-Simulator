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
        //float distanceTo(const Node*); //remove?
        //virtual void recievePacket(std::unique_ptr<Packet>); //UNCOMMENT
    
    private:
        int id;
        int x;
        int y;
        


};

#endif