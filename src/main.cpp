#include <iostream>
#include "UE.h"
#include "Packet.h"
#include <string>
#include "Tester.h"
#include "gNB.h"
#include <vector>

int main() {

    std::cout << '\n' << '\n' << '\n';

    //std::cout << "C++ version: " << __cplusplus << std::endl;

    UE testUE{1001, 5, 5};
    gNB testGNB {5001, 5, -60};   //distance 65
    gNB testGNB2 {5002, -2, -2};  //9.90 -> should connect to this 
    gNB testGNB3 {5003, -20, 3};  //25.08
    

    std::vector<gNB*> gnbList;
    gnbList.push_back(&testGNB);
    gnbList.push_back(&testGNB2);
    gnbList.push_back(&testGNB3);

    

    // std::cout << "UE #" << testUE.getID() << " @ " << testUE.getLocation().first << "," << testUE.getLocation().second << std::endl;
    // std::cout << "gNB #" << testGNB.getID() << " @ " << testGNB.getLocation().first << "," << testGNB.getLocation().second << std::endl;

    std::cout << '\n' << '\n';
    
    
    //testUE.sendPacket(5001, 1, PacketType::ACK, testString, 1);

    testUE.turnOn(gnbList); //turn on UE and connect to gnb


    std::string testString {"Hello World!"};
    

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
    

    
    

    std::cout << '\n' << '\n' << '\n';

    return 0;
}