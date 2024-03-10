#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CONFIG ".\\files\\config.txt"
// 定义配置项结构
typedef struct {
    char* name;
    char* value;
} ConfigItem;

// 定义链表节点
typedef struct Node {
    ConfigItem item;
    struct Node* next;
} Node;

// 定义链表头节点
Node* head = NULL;


void addConfigItem(const char* name, const char* value);
void readConfigFile();
void saveConfigFile();
void cleanupConfigList();
void deleteConfigItem(const char* name);
void replaceConfigOption(const char* option, const char* value);
void printAllConfigItems();
void initconfig();
const char* getConfigValue(const char* name);

void initconfig() {
    FILE* file = fopen(CONFIG, "r");
    if (file == NULL) {
        addConfigItem("namesfile", ".\\names.txt");
        addConfigItem("language", "chinese");
        addConfigItem("Diamond", "10000");
        addConfigItem("buleball", "10000");
        addConfigItem("redball", "10000");
        addConfigItem("off video", "0");
        addConfigItem("first mode", "1");
        saveConfigFile();
        readConfigFile();
        printAllConfigItems();
    }
    else
    {
        fclose(file);
        readConfigFile();
        printAllConfigItems();
    }
    log("config init successfully");
}
const char* getConfigValue(const char* name) {
    Node* current = head;

    // 遍历链表查找匹配的配置项
    while (current != NULL) {
        if (strcmp(current->item.name, name) == 0) {
            // 找到匹配的配置项，返回其参数值
            return current->item.value;
        }
        current = current->next;
    }

    // 如果没有找到匹配的配置项，返回空字符串或适当的默认值
    return "err";
}
// 添加配置项到链表
void addConfigItem(const char* name, const char* value) {
    Node* current = head;
    Node* prev = NULL;
    while (current != NULL) {
        if (strcmp(current->item.name, name) == 0) {
            strcpy(current->item.value, value);
            return;
        }
        prev = current;
        current = current->next;
    }
    ConfigItem newItem;
    newItem.name = _strdup(name);
    newItem.value = _strdup(value);

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        errlog("Memory allocation error(addConfigItem)");
    }

    newNode->item = newItem;
    newNode->next = head;
    head = newNode;
    saveConfigFile();
}

// 读取配置文件并保存配置项到链表
void readConfigFile() {
    FILE* file = fopen(CONFIG, "r");
    if (file == NULL) {
        errlog("Error opening file for reading");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        char* trimLine = strtok(line, "\n");  // 移除换行符
        char* currentOption = strtok(trimLine, "=");
        if (currentOption != NULL) {
            char* value = strtok(NULL, "=");
            // 添加配置项到链表
            addConfigItem(currentOption, value);
            log(trimLine);
        }
    }
    fclose(file);
}

// 保存配置项到配置文件
void saveConfigFile() {
    FILE* file = fopen(CONFIG, "w");
    if (file == NULL) {
        errlog("Error opening file for writing");
        return;
    }
    Node* current = head;
    while (current != NULL) {
        fprintf(file, "%s=%s\n", current->item.name, current->item.value);
        current = current->next;
    }
    fclose(file);
}

// 释放链表节点的内存
void cleanupConfigList() {
    Node* current = head;
    Node* next;
    while (current != NULL) {
        next = current->next;
        free(current->item.name);
        free(current->item.value);
        free(current);
        current = next;
    }
    head = NULL;
    log("Free ConfigList complete");
}


// 删除配置项
void deleteConfigItem(const char* name) {
    Node* prev = NULL;
    Node* current = head;
    while (current != NULL) {
        if (strcmp(current->item.name, name) == 0) {
            // 找到匹配的配置项，删除它
            if (prev != NULL) {
                prev->next = current->next;
            }
            else {
                head = current->next;
            }
            free(current->item.name);
            free(current->item.value);
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }
    log("del %s success", name);
}
// 替换配置文件中的指定配置项
void replaceConfigOption(const char* option, const char* value) {
    FILE* file = fopen(CONFIG, "r");
    if (file == NULL) {
        errlog("Error opening file for reading");
        return;
    }

    const char* tempFilename = "temp_config.txt";
    FILE* tempFile = fopen(tempFilename, "w");
    if (tempFile == NULL) {
        errlog("Error opening temporary file for writing");
        fclose(file);
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        char* trimLine = strtok(line, "\n");  // 移除换行符
        char* currentOption = strtok(trimLine, "=");
        if (currentOption != NULL && strcmp(currentOption, option) == 0) {
            // 替换现有配置项
            fprintf(tempFile, "%s=%s\n", option, value);
        }
        else {
            // 保留其他配置项
            fprintf(tempFile, "%s\n", trimLine);
        }
    }
    fclose(file);
    fclose(tempFile);
    // 替换原始文件
    remove(CONFIG);
    rename(tempFilename, CONFIG);
}

void printAllConfigItems() {
    log(" Configuration:");
    Node* current = head;
    while (current != NULL) {
        log("%s: %s", current->item.name, current->item.value);
        current = current->next;
    }
}