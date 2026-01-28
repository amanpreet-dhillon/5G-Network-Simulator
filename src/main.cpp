#include <iostream>
#include "UE.h"
#include "Packet.h"
#include <string>
#include "Tester.h"
#include <vector>

int main() {

    std::cout << '\n' << '\n' << '\n';

    //std::cout << "C++ version: " << __cplusplus << std::endl;

    UE testUE{1001, 5, 5};
    Tester testGNB {5001, 10, 7};

    std::vector<Tester*> gnbList;
    gnbList.push_back(&testGNB);

    std::cout << "UE #" << testUE.getID() << " @ " << testUE.getLocation().first << "," << testUE.getLocation().second << std::endl;
    std::cout << "gNB #" << testGNB.getID() << " @ " << testGNB.getLocation().first << "," << testGNB.getLocation().second << std::endl;

    std::cout << '\n' << '\n';
    
    std::string testString {"Hello World!"};
    //testUE.sendPacket(5001, 1, PacketType::ACK, testString, 1);

    testUE.turnOn(gnbList); //turn on UE and connect to gnb

    /*
    //send a packet and get an ack back -----> WORKS
    testString = "send packet, get ACK";
    testUE.sendPacket(5001, 1, PacketType::DATA, testString, 0);

    testString = "ACK packet";
    testGNB.sendPacket(1001, 1, PacketType::ACK, testString, -1);
    */


    //testing for recieveing packet functionality 
    // seq == expected seq ---> WORKS
    // seq < expected seq ---> WORKS
    // seq > expected seq ---> WORKS

    /*
    testString = "establishing recievingTracker map";
    testGNB.sendPacket(1001, 1, PacketType::DATA, testString, 1);

    testString = "testing for case seq < eSeq";
    testGNB.sendPacket(1001, 4, PacketType::DATA, testString, 1);

    testString = "sending seq == eSeq";
    testGNB.sendPacket(1001, 2, PacketType::DATA, testString, 1);

    testString = "testing SKIP flag";
    testGNB.sendPacket(1001, -1, PacketType::SKIP, testString, 1);

    */

    testString = "testing retransmission queue";
    // testUE.sendPacket(5001, 1, PacketType::DATA, testString, 0);
    // testGNB.sendPacket(1001, 1, PacketType::ACK, testString, -1);
    
    // testUE.sendPacket(5001, 2, PacketType::DATA, testString, 0);
    testUE.sendPacket(5001, 1, PacketType::DATA, testString, 0);
    
    // testGNB.sendPacket(1001, 2, PacketType::ACK, testString, -1);

    // testUE.sendPacket(5001, 4, PacketType::DATA, testString, 0);
    // testGNB.sendPacket(1001, 4, PacketType::ACK, testString, -1);

    testString = "testing NACK";
    // testGNB.sendPacket(1001, 5, PacketType::NACK, testString, 1);

    testGNB.sendPacket(1001, 5, PacketType::NACK, testString, 1);


    std::cout << '\n' << '\n' << '\n';

    return 0;
}