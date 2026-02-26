#include <iostream>
#include "UE.h"
#include "Packet.h"
#include <string>
#include "Tester.h"
#include "gNB.h"
#include <vector>
#include "CoreNetwork.h"
#include "NetworkSimulator.h"

int main() {

    std::cout << '\n' << '\n' << '\n';

    //std::cout << "C++ version: " << __cplusplus << std::endl;


    NetworkSimulator simulator;

    simulator.startSimulation(5);


    /*
    CoreNetwork* coreNetwork = new CoreNetwork();


    std::vector<std::unique_ptr<Node>> UEs;
    std::vector<std::unique_ptr<Node>> gNBs;

    std::vector<UE*> ueObservers;
    std::vector<gNB*> gnbObservers;


    auto ue = std::make_unique<UE>(1001, 5, -58);
    auto ue2 = std::make_unique<UE>(1002, -50, 20);
    auto gnb = std::make_unique<gNB>(5001, 5, -60);
    auto gnb2 = std::make_unique<gNB>(5002, 27, 10);
    
    gnb->establishConnectionToCore(coreNetwork);
    gnb2->establishConnectionToCore(coreNetwork);

    ueObservers.push_back(ue.get());
    ueObservers.push_back(ue2.get());
    gnbObservers.push_back(gnb.get());
    gnbObservers.push_back(gnb2.get());

    UEs.push_back(std::move(ue));
    UEs.push_back(std::move(ue2));
    gNBs.push_back(std::move(gnb));
    gNBs.push_back(std::move(gnb2));

    
    coreNetwork->loadEquipment(std::move(UEs), std::string("UE"));
    coreNetwork->loadEquipment(std::move(gNBs), std::string("gNB"));
    

    ueObservers[0]->turnOn(gnbObservers);
    ueObservers[1]->turnOn(gnbObservers);
    

    ueObservers[0]->sendPacket(9999, PacketType::DATA, std::string("Test packet 1"), 0);

    ueObservers[0]->sendPacket(1002, PacketType::DATA, std::string("sending a packet to existing UE"), 0);

    ueObservers[1]->turnOff();

    ueObservers[0]->sendPacket(1002, PacketType::DATA, std::string("sending a packet to non existing UE"), 0);

    */


    // UE testUE{1001, 5, 5};
    // gNB testGNB {5001, 5, -60};   //distance 65
    // gNB testGNB2 {5002, -2, -2};  //9.90 -> should connect to this 
    // gNB testGNB3 {5003, -20, 3};  //25.08
    

    // std::vector<gNB*> gnbList;
    // gnbList.push_back(&testGNB);
    // gnbList.push_back(&testGNB2);
    // gnbList.push_back(&testGNB3);

    

    // std::cout << "UE #" << testUE.getID() << " @ " << testUE.getLocation().first << "," << testUE.getLocation().second << std::endl;
    // std::cout << "gNB #" << testGNB.getID() << " @ " << testGNB.getLocation().first << "," << testGNB.getLocation().second << std::endl;

    std::cout << '\n' << '\n';
    
    
    //testUE.sendPacket(5001, 1, PacketType::ACK, testString, 1);

    //testUE.turnOn(gnbList); //turn on UE and connect to gnb


    std::string testString {"Hello World!"};
    
    /* UE TESTING
    //testing send

    // //basic send
    // testString = "testing basic send";
    // testUE.sendPacket(1005, 0, PacketType::DATA, testString, 0);

    // //ACK send
    // testString = "testing ACK sending of a recieved packet";
    // auto testPacket = std::make_unique<Packet>(1005, 1001, 1, PacketType::DATA, testString, 0);
    // testUE.recievePacket(std::move(testPacket));

    // //NACK send
    // testString = "testing NACK sending of a recieved packet";
    // testGNB2.tester_addPacketToReQueue(1001, 2);
    // testGNB2.tester_addPacketToReQueue(1001, 3);
    // testGNB2.tester_addPacketToReQueue(1001, 4);
    // auto testPacket2 = std::make_unique<Packet>(1005, 1001, 5, PacketType::DATA, testString, 0);
    // testUE.recievePacket(std::move(testPacket2));

    // //SKIP send
    // testString = "testing SKIP sending of UE";
    // //auto testPacket3 = std::make_unique<Packet>(5002, 1001, 8, PacketType::NACK, testString, 0);
    // testGNB2.sendPacket(1001, 5002, 8, PacketType::NACK, testString, 1);


    
    
    //testing recieve 

    //basic recieve
    // testString = "testing basic recieve";
    // auto testPacket = std::make_unique<Packet>(1005, 1001, 1, PacketType::DATA, testString, 0);
    // testUE.recievePacket(std::move(testPacket));


    // //ACK recieve
    // testString = "testing ACK recieve";
    // testUE.tester_addPacketToReQueue(1005, 1);
    // auto testPacket = std::make_unique<Packet>(1005, 1001, 1, PacketType::ACK, testString, 0);
    // testUE.recievePacket(std::move(testPacket));


    // //NACK recieve
    // testString = "test";
    // testUE.sendPacket(1005, 1, PacketType::DATA, testString, 0);

    // testString = "test 2";
    // testUE.sendPacket(1005, 3, PacketType::DATA, testString, 0);

    // testUE.tester_addPacketToReQueue(1005, 4);
    // testString = "test 3";
    // testUE.sendPacket(1005, 5, PacketType::DATA, testString, 0);


    // //SKIP recieve
    // testString = "testing SKIP recieve of UE";
    // //auto testPacket3 = std::make_unique<Packet>(5002, 1001, 8, PacketType::NACK, testString, 0);
    // testGNB2.sendPacket(1001, 5002, 8, PacketType::NACK, testString, 1);
    
    */
    

    //gNB testing

    //sending

    // //send basic
    // testString = "test basic send";
    // auto testPacket = std::make_unique<Packet>(1005, 1001, 1, PacketType::DATA, testString, 0);
    // testGNB2.sendPacket(1001, std::move(testPacket));

    
    // //send ACK
    // testString = "test send ACK";
    // testUE.sendPacket(1005, 1, PacketType::DATA, testString, 0);

    // //send NACK
    // testString = "test send NACK";
    // testUE.sendPacket(1005, 5, PacketType::DATA, testString, 0);


    //send SKIP
    // testString = "testing SKIP send";
    // testUE.sendPacket(1005, 5, PacketType::NACK, testString, 1);



    //recieve


    //get basic
    // testString = "test basic recieve";
    // testUE.sendPacket(1005, 1, PacketType::DATA, testString, 0);

    // //get ACK
    // testString = "test ACK recieve";
    // testGNB2.tester_addPacketToReQueue(1001, 1);
    // testUE.sendPacket(5002, 1, PacketType::ACK, testString, 0);


    //get NACK
    // testString = "test NACK recieve";
    // testGNB2.tester_addPacketToReQueue(1001, 1);
    // testUE.sendPacket(5002, 1, PacketType::NACK, testString, 0);

    //get SKIP
    // testString = "test SKIP recieve";
    // testUE.sendPacket(5002, 5, PacketType::SKIP, testString, 1);

    std::cout << '\n' << '\n' << '\n';

    return 0;
}