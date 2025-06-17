#pragma once

#include <string>
#include <vector>
#include "log.h"
#include <minizip/unzip.h>

class unzip{
    public:
        unzip(const std::string& zipFile);
        ~unzip();

        bool extract(const std::string& destDir);
    private:
        std::string zipFilePath;
        std::vector<std::string> extractedFiles;
        unzFile zipFileHandle = nullptr;
        std::string upgraderFile;
};