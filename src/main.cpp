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
    
    std::string testString {"Hello World!"};
    //testUE.sendPacket(5001, 1, PacketType::ACK, testString, 1);

    testUE.turnOn(gnbList); //turn on UE and connect to gnb

    
    //send a packet and get an ack back -----> WORKS
    testString = "testing";
    testUE.sendPacket(5002, 1, PacketType::DATA, testString, 0);
    testUE.sendPacket(5002, 2, PacketType::DATA, testString, 0);
    testUE.sendPacket(5002, 3, PacketType::DATA, testString, 0);


    //testString = "ACK packet";
    //testGNB.sendPacket(1001, 1, PacketType::ACK, testString, -1);
    


    //testing for recieveing packet functionality 
    // seq == expected seq ---> WORKS
    // seq < expected seq ---> WORKS
    // seq > expected seq ---> WORKS

    
    //testString = "establishing recievingTracker map";
    //testGNB.sendPacket(1001, 1, PacketType::DATA, testString, 1);

    testString = "testing for case seq < eSeq";
    //testUE.sendPacket(1001, 1, PacketType::DATA, testString, 1);

    testString = "sending seq == eSeq";
    //testUE.sendPacket(1001, 4, PacketType::DATA, testString, 1);

    testString = "sending seq > eSeq";
    //testUE.sendPacket(1001, 5, PacketType::DATA, testString, 1);
    //testUE.sendPacket(1001, 4, PacketType::DATA, testString, 1);


    testString = "testing SKIP flag";
    testUE.sendPacket(5002, -1, PacketType::SKIP, testString, 1);
    testUE.sendPacket(5002, 6, PacketType::DATA, testString, 1);
    testUE.sendPacket(5002, 5, PacketType::DATA, testString, 1);
    

    // testString = "testing retransmission queue";
    // // testUE.sendPacket(5001, 1, PacketType::DATA, testString, 0);
    // // testGNB.sendPacket(1001, 1, PacketType::ACK, testString, -1);
    
    // // testUE.sendPacket(5001, 2, PacketType::DATA, testString, 0);
    // testUE.sendPacket(5001, 1, PacketType::DATA, testString, 0);
    
    // // testGNB.sendPacket(1001, 2, PacketType::ACK, testString, -1);

    // // testUE.sendPacket(5001, 4, PacketType::DATA, testString, 0);
    // // testGNB.sendPacket(1001, 4, PacketType::ACK, testString, -1);

    // testString = "testing NACK";
    // // testGNB.sendPacket(1001, 5, PacketType::NACK, testString, 1);

    // testGNB.sendPacket(1001, 5, PacketType::NACK, testString, 1);


    std::cout << '\n' << '\n' << '\n';

    return 0;
}