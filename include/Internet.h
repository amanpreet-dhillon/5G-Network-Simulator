#ifndef INTERNET_H
#define INTERNET_H
#include <string>
#include <fstream>
#include <vector>

class Internet{

    public:
        Internet();
        ~Internet();
        std::string getRandomResponse();
        void recieveData(const std::string&);

    private:
        std::ofstream outputFile;
        std::ifstream inputFile;
        std::vector<std::string> internetLines;
        void readLines();
};

#endif