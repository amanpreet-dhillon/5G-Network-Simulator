#include "Internet.h"
#include <filesystem>
#include <iostream>
#include "Utils.h"

Internet::Internet(){

    //outputFile.open("./output/Data_from_UEs.txt", std::ios::app); //for appending
    outputFile.open("./output/Data_from_UEs.txt");
    inputFile.open("./input/Internet.txt");

    if(!inputFile.is_open()){
        std::cerr << "Error: could not open Internet.txt\n";
    }
    
    if(!outputFile.is_open()){
        std::cerr << "Error: could not open Data_from_UEs.txt\n";
    }

    readLines();
    inputFile.close();

}

Internet::~Internet(){
    if (inputFile.is_open()) {inputFile.close();}
    if (outputFile.is_open()) {outputFile.close();}
}


void Internet::recieveData(const std::string& data){
    if(outputFile.is_open()){
        outputFile << data << std::endl;
    }
}

std::string Internet::getRandomResponse(){
    if(internetLines.size() >= 1){
        int index = Utils::generateRandNum(0, internetLines.size()-1);
        return internetLines[index];
    } else {
        return std::string("Default text");
    }
}

void Internet::readLines(){
    if(inputFile.is_open()){
        std::string line;
        while (getline(inputFile, line)){
            internetLines.push_back(line);
        }
    }
}