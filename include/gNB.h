#ifndef GNB_H
#define GNB_H

#include "Node.h"
#include "CoreNetwork.h" 
#include "Packet.h"
#include <vector>
#include <queue>
#include <map>

class UE;

class gNB : public Node {
    public:
        gNB(int,int,int);
        ~gNB();
        void connectUE(UE*);
        void disconnectUE(UE*);
        void recievePacket(std::unique_ptr<Packet>);
        void recievePacket(std::unique_ptr<GTPPacket>);
        void forwardToCore(std::unique_ptr<GTPPacket>);
        void establishConnectionToCore(const CoreNetwork*);
        void sendPacket(int, int, PacketType, const std::string&, int);
        //void disconnectUE(int);   /remove?


    private:
        const CoreNetwork* coreNetwork;
        std::vector<UE*> connectedUEs;
        //std::priority_queue<Packet> packets;
        std::map<int, int> recievedPacketTracker; //keep track of packets recieved -- source ID / expected sequence
        std::map<std::pair<int,int>, std::unique_ptr<Packet>> retransmissionQueue;  //stores packets sent to a specific destination  -- destination ID, expected sequence pair / packet sent
        std::map<int, int> destinationSeqTracker; //keep track of the number of packets sent to a specific destination ID (i.e. the sequence number)
        std::map<std::pair<int,int>, std::unique_ptr<Packet>> buffer;
        void bufferCleanUp(int);
        

};

#endif