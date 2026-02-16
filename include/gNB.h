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

struct GTPPacket {
    int TEID;   //TEIDs will be coded as 8XXXX (UL) or 4XXXX (DL) where XXXX = UE ID,
    std::unique_ptr<Packet> payload;
};

struct UEContext {
    UE* ue = nullptr;   
    
    int nextDLSeq = 0;
    std::unordered_map<int, std::unique_ptr<Packet>> retransmissionQueue;   //seq, Packet
    
    int expectedULSeq = 1;
    std::map<int, std::unique_ptr<Packet>> buffer;  //seq, Packet
};


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
        void tester_addPacketToReQueue(int, int);
        void sendPacket(int, int, int, PacketType, const std::string&, int);
        void sendPacket(int, std::unique_ptr<Packet>);


    private:
        std::unordered_map<int, std::unique_ptr<UEContext>> ueRegistery;    //stores all UEContexts currently connected to gNB
        void forwardToCore(std::unique_ptr<Packet>);
        const CoreNetwork* coreNetwork; //connected Core Network
        std::unordered_map<int, int> dl_TEIDs;  //TEID maps to UE ID
        std::unordered_map<int, int> ul_TEIDs;  //UE ID maps to TEID
        void bufferCleanUp(int);


};



#endif