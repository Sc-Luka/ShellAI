#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <mutex>
class Logger{
private:
    std::ofstream logFile;
    std::mutex mtx;
    std::string getTimestamp(){
        auto now=std::chrono::system_clock::now();
        std::time_t now_time=std::chrono::system_clock::to_time_t(now);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));
        return std::string(buf);
    }
    void writeLog(const std::string& level, const std::string& message){
        std::lock_guard<std::mutex> lock(mtx);
        if(logFile.is_open()){
            logFile << "[" << getTimestamp() << "] " << "[" << level << "] "
                                                    << message << std::endl;
        }
    }
public:
    Logger(const std::string& filename = "ai_terminal.log") {
        logFile.open(filename, std::ios::app); // append mode
        if (!logFile.is_open()) 
            std::cerr << "Failed to open log file: " << filename << "\n";
        }
    ~Logger(){if(logFile.is_open())logFile.close();}

    void user(const std::string& msg) { writeLog("USER", msg); }
    void ai(const std::string& msg) { writeLog("AI", msg); }
    void info(const std::string& msg) { writeLog("INFO", msg); }
    void warning(const std::string& msg) { writeLog("WARNING", msg); }
    void error(const std::string& msg) { writeLog("ERROR", msg); }
    void output(const std::string& msg) { writeLog("EXECUATE", msg); }

};