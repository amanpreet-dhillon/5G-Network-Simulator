#ifndef GNB_H
#define GNB_H

#include "Node.h"
#include "CoreNetwork.h" 
#include "Packet.h"
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>

class UE;

class gNB : public Node {
    public:
        gNB(int,int,int);
        ~gNB();
        void connectUE(UE*);
        void disconnectUE(UE*);
        void recievePacket(std::unique_ptr<Packet>);
        void recievePacket(std::unique_ptr<GTPPacket>);
        
        void establishConnectionToCore(const CoreNetwork*);
        
        void recieveUL_TEID(int, int);
        //void disconnectUE(int);   /remove?


    private:
        void forwardToCore(std::unique_ptr<Packet>);
        void sendPacket(int, int, int, PacketType, const std::string&, int);
        void sendPacket(int, std::unique_ptr<Packet>);
        const CoreNetwork* coreNetwork;
        std::unordered_map<int, UE*> connectedUEs;
        std::unordered_map<int, int> dl_TEIDs;  //TEID maps to UE ID
        std::unordered_map<int, int> ul_TEIDs;  //UE ID maps to TEID
        //std::priority_queue<Packet> packets;
        std::map<std::pair<int,int>, int> recievedPacketTracker; //keep track of packets recieved -- {source ID, destination ID} / expected sequence
        std::map<std::pair<std::pair<int,int>, int>, std::unique_ptr<Packet>> retransmissionQueue;  //stores packets sent to a specific destination  -- {{source ID, destination ID}, expected sequence pair} / packet sent
        std::map<std::pair<int,int>, int> destinationSeqTracker; //keep track of the number of packets sent to a specific destination ID (i.e. the sequence number) -> {source ID, destination ID} / seq
        std::map<std::pair<int, int>, std::unique_ptr<Packet>> buffer;   //store out-of-order packets -> {source ID, destination ID} / packet 
        void bufferCleanUp(int, int);


};

#endif