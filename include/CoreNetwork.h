#ifndef CORENETWORK_H
#define CORENETWORK_H

#include "Node.h"
#include "Packet.h"
#include <queue>
#include <unordered_map>
#include "UE.h"
#include <vector>
#include <string>
#include <sstream>

class gNB;

struct UESession {
    UE* ue;
    int connectedgNBID;
    int ul_TEID;
    int dl_TEID;
};

class CoreNetwork {
    public:
        CoreNetwork();
        ~CoreNetwork();
        void recievePacket(std::unique_ptr<GTPPacket>);
        void recieveDL_TEID(int, int);
        void loadEquipment(std::vector<std::unique_ptr<Node>>, const std::string&);
        void removeUE(int);
        static constexpr int INTERNET = 9999;
        static constexpr int CORE_NETWORK = 0;
    
    private:
        std::unordered_map<int, std::unique_ptr<gNB>> gNBRegistry;  //stores gNB pointers of simulation
        std::unordered_map<int, std::unique_ptr<UE>> ueRegistry;    //stores UE pointers of simulation
        std::unordered_map<int, UE*> activeUEConnections;    //stores UE pointers of simulation
        std::unordered_map<int, int> dl_TEIDs;  //UE ID maps to TEID
        std::unordered_map<int, int> ul_TEIDs;  //TEID maps to UE ID
        std::unordered_map<int, int> ue_gNb_connection; //key: ue ID, value: gNB ID it is connected to
        //std::priority_queue<Packet> packetsFrom_gNB;
        //int id;
        //void registerUE(std::unique_ptr<UE>);
        //void registergNB(std::unique_ptr<gNB>);
        void forwardPacketToUE(int, std::unique_ptr<Packet>);
        bool AMF_authenticateUE(int, const std::string&);
        std::vector<std::string> split(const std::string&, char);


};

#endif