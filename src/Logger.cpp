#include "Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

Logger::Logger(){
    logOutput.open("./output/SimulationLog.txt");

    if(!logOutput.is_open()){
        std::cerr << "Error: cannot open Simulation.log" << std::endl;
    }

}

Logger::~Logger(){
    if(logOutput.is_open()) {logOutput.close();}
}

Logger& Logger::getInstance(){
    static Logger instance;
    return instance;
}

void Logger::logPacketSent(int source, const std::string& data){

    std::lock_guard<std::mutex> lock(logMutex); //mutex to esnure stable writing to file between multiple equipments

    if (logOutput.is_open()) {
        logOutput << "[" << getTimestamp() << "] --- [" << getEquipmentType(source) << "]" << " SENT: " << data << std::endl;
    }
}

void Logger::logPacketRecieved(int source, const std::string& data){

    std::lock_guard<std::mutex> lock(logMutex); //mutex to esnure stable writing to file between multiple equipments

    if (logOutput.is_open()) {
        logOutput << "[" << getTimestamp() << "] --- [" << getEquipmentType(source) << "]" << " RECEIVED: " << data << std::endl;
    }
}

void Logger::logOther(int source, const std::string& data){

    std::lock_guard<std::mutex> lock(logMutex); //mutex to esnure stable writing to file between multiple equipments

    if (logOutput.is_open()) {
        logOutput << "[" << getTimestamp() << "] --- [" << getEquipmentType(source) << "]" << data << std::endl;
    }
}

std::string Logger::getTimestamp(){
    auto now = std::chrono::system_clock::now();

    std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);

    std::tm* locale_tm = std::localtime(&time_t_now);

    std::stringstream ss;
    ss << std::put_time(locale_tm, "%Y/%m/%d - %H:%M:%S");

    return ss.str();
}

std::string Logger::getEquipmentType(int equipID){
    
    std::string source_str = std::to_string(equipID);
    std::string equipmentType;

    if(source_str.at(0) == '1'){
        equipmentType = "UE " + source_str;
    } else if(source_str.at(0) == '5'){
        equipmentType = "gNB " + source_str;
    } else if (source_str.at(0) == '9'){
        equipmentType = "INTERNET";
    } else if (source_str.at(0) == '0'){
        equipmentType = "CORE NETWORK";
    } else {
        equipmentType = "SIMULATOR";
    }

    return equipmentType;
}