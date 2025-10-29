#ifndef EXPORT_HPP
#define EXPORT_HPP

#pragma once

#include "../MyDefine.h"
#include "../TheMemory.h"
#include "../TheTools.hpp"
#include "../GameDefinition.h"
#include "../TheSdCard.hpp"

#ifndef ESP32P4

class Export {
    public:
    Export();
    ~Export();
        void GenerateAll(TheSdCard &sdCard);

    private:
        const char *TAG = "Export";
        std::string JsonTagValue(std::string  tag, const char *value, bool comma = true);
        std::string JsonTagHexValue(std::string tag, unsigned long value, bool comma = true);
        std::string JsonTagNumber(std::string tag, unsigned long value, bool comma = true);

};
#endif

#endif