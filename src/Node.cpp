#include "Node.h"
#include "Utils.h"



Node::Node(int givenID, int xCoord, int yCoord)
        : id {givenID}, x {xCoord}, y {yCoord} 
{
    
}

Node::~Node(){
    
}

int Node::getID(){
    return id;
}

std::pair<int, int> Node::getLocation() const{
    return std::pair{x,y};
}


