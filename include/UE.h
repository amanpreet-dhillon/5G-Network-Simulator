#ifndef UE_H
#define UE_H

#include <vector>
#include <unordered_map>
#include "Node.h"
#include "Utils.h"




class gNB;


class UE : public Node {
    public:
        UE(int, int, int);
        ~UE();
        void recievePacket(std::unique_ptr<Packet>);
        void sendPacket();
        void turnOn(std::vector<gNB*>);
        void turnOff();
        //void sendRegistrationRequest();
        //void sendConfirmationRecieved();

    private:
        gNB* connected_gNB;
        bool active;
        Utils util;
        std::unordered_map<int, int> recievedTracker;
        //std::unordered_map<int, int> sentTracker;
        

};

#endif