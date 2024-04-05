#include "config2.h"
Config::Config(std::string conFigFile) :configfile(conFigFile) {
    head = new ConfigNode;
    head->next = NULL;
    tail = head;
}

Config::~Config() {
    ConfigNode* p = head;
    ConfigNode* q;
    while (p) {
        q = p->next;
        delete p;
        p = q;
    }
}

void Config::readConfig() {
    std::ifstream infile(configfile);
    if (!infile) {
        std::cerr << "Error: unable to open config file: " << configfile << std::endl;
    }
    std::string line;
    while (getline(infile, line)) {
        if (line[0] == '#') {
            continue;
        }
        std::string key = line.substr(0, line.find('='));
        std::string value = line.substr(line.find('=') + 1);
        ConfigNode* node = new ConfigNode;
        node->key = key;
        node->value = value;
        node->next = NULL;
        tail->next = node;
        tail = node;
    }
    infile.close();
}

void Config::SaveConfig() {
    std::ofstream outfile(configfile);
    if (!outfile) {
        std::cerr << "Error: unable to open config file: " << configfile << std::endl;
    }
    ConfigNode* p = head->next;
    while (p) {
        outfile << p->key << "=" << p->value << std::endl;
        p = p->next;
    }
    outfile.close();
}

void Config::setConfig(std::string key, std::string value) {
    ConfigNode* p = head->next;
    while (p) {
        if (p->key == key) {
            p->value = value;
            return;
        }
        p = p->next;
    }
    ConfigNode* node = new ConfigNode;
    node->key = key;
    node->value = value;
    node->next = NULL;
    tail->next = node;
    tail = node;
}

std::string Config::getConfig(std::string key) {
    ConfigNode* p = head->next;
    while (p) {
        if (p->key == key) {
            return p->value;
        }
        p = p->next;
    }
    return "err";
}

void Config::printConfig() {
    ConfigNode* p = head->next;
    while (p) {
        std::cout << p->key << "=" << p->value << std::endl;
        p = p->next;
    }
}