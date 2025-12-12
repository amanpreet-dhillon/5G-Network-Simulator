#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <chrono>

class Packet {

    public:
        Packet();
        ~Packet();
        int getPriority();
    
    private:
        std::string data;
        std::time_t timestamp;
        int priority;


};

#endif