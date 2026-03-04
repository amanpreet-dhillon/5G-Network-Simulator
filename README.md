# 5G-Network-Simulator
Using C++, this project simulates 5G network traffic (only a simulation, no hardware involved).
The reason for making this project was for me to learn the logic behind how packets are being sent and received by key components (e.g. UEs and gNBs) within a 5G network.
I wanted to use this project as a way for me to get interested in networking and telecommunication in general and it was fun to build. 

To run this project: make run
To clean binaries and build: make clean
to clean output directory (i.e. the logs): make clean-logs

You can increase the number of iterations the simulation does by changing the value of the number_of_iterations variable in main.cpp.
Comment out the 'sleep' line in NetworkSimulation.cpp to make the program run quicker, otherwise it is meant to help show any changes in UE movements which is printed to the terminal as a Cartesian grid. 


Made by Amanpreet Dhillon