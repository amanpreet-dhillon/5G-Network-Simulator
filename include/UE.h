#ifndef UE_H
#define UE_H

#include <memory>
#include "Node.h"

class gNB;


class UE : public Node {
    public:
        UE();
        ~UE();
        void recievePacket(const Packet&);
        void sendPacket();

    private:
        std::unique_ptr<gNB> connected_gNB;
        

};

#endif