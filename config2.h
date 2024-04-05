#pragma once
#include <iostream>
#include <fstream>
#include <string>

struct ConfigNode {
    std::string key;
    std::string value;
    ConfigNode* next;
};

class Config
{
public:
    Config(std::string configfile);
    ~Config();
    void readConfig();
    void SaveConfig();
    void setConfig(std::string key, std::string value);
    std::string getConfig(std::string key);
    void printConfig();
private:
    ConfigNode* head;
    ConfigNode* tail;
    //≈‰÷√Œƒº˛
    std::string configfile;
};

