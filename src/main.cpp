#include <iostream>
#include "UE.h"
#include "Packet.h"
#include <string>

int main() {

    std::cout << '\n' << '\n' << '\n';

    std::cout << "C++ version: " << __cplusplus << std::endl;

    UE testUE{1001, 5, 5};

    std::cout << testUE.getID() << '\n';
    std::cout << "X:" << testUE.getLocation().first << " Y:" << testUE.getLocation().second << '\n';
    
    std::string testString {"Hello World!"};

    testUE.sendPacket(5001, PacketType::ACK, testString, 1);


    std::cout << '\n' << '\n' << '\n';

    return 0;
}