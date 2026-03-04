#include <iostream>
#include "UE.h"
#include "Packet.h"
#include <string>
#include "gNB.h"
#include <vector>
#include "CoreNetwork.h"
#include "NetworkSimulator.h"

int main() {

    std::cout << '\n' << '\n' << '\n';

    int number_of_iterations {5};   //change this value to change how many times simulation iterates/loops

    NetworkSimulator simulator;
    simulator.startSimulation(number_of_iterations);

    std::cout << '\n' << '\n' << '\n';

    return 0;
}