#ifndef UNZIP_H
#define UNZIP_H
#pragma once
#include "Log.h"
#include <string>
class unzip
{
public:
    static std::string upgraderfile;
    // 将字符串内的old_value替换成new_value
    static std::string& replace_all(std::string& str, const std::string& old_value, const std::string& new_value);
    // 创建多级目录
    static bool CreatedMultipleDirectory(const std::string& direct);
    /*
    * 函数功能 : 递归解压文件目录
    * 备    注 : strFilePath 压缩包路径
    *      strTempPath 解压到
    */
    static void UnzipFile(const std::string& strFilePath, const std::string& strTempPath);
};

#endif // !UNZIP_H