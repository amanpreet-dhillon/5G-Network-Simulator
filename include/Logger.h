#ifndef LOGGER_H
#define LOGGER_H
#include <fstream>
#include <mutex>
#include <string>

//SINGLETON CLASS
class Logger{

    public:
        ~Logger();
        static Logger& getInstance();
        Logger(const Logger&) = delete; //prevent copying instance of Logger
        void operator=(const Logger&) = delete; //disable = operator, preventing overwriting 
        
        void logPacketSent(int, const std::string&);
        void logPacketRecieved(int, const std::string&);
        void logOther(int, const std::string&);

    private:
        Logger();
        std::ofstream logOutput;
        std::string getTimestamp();
        std::mutex logMutex;
        std::string getEquipmentType(int);
};

#endif