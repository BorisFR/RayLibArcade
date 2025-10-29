#include "Export.hpp"

Export::Export() { MY_DEBUG(TAG, "created"); }

Export::~Export() { MY_DEBUG(TAG, "destroyed") }

void Export::GenerateAll(TheSdCard &sdCard)
{
    setlocale(LC_ALL, "fr_FR.UTF-8");
    FILE *file = NULL;
    file = sdCard.CreateJsonFileOnWiki("gamelist");
    sdCard.FileWriteText(file, "{\n  \"games\": [");
    bool comma = false;
    for (uint8_t g = 1; g < countGames; g++) // ignore first, it is themenu
    {
        if (comma)
        {
            sdCard.FileWriteText(file, ",");
        }
        else
        {
            comma = true;
        }
        sdCard.FileWriteText(file, "\n    {");
        std::string temp = JsonTagValue("id", allGames[g].folder);
        sdCard.FileWriteText(file, temp.c_str());
        temp = JsonTagValue("name", allGames[g].name, false);
        sdCard.FileWriteText(file, temp.c_str());
        sdCard.FileWriteText(file, "}");
    }
    sdCard.FileWriteText(file, "\n  ]\n");
    sdCard.FileWriteText(file, "}\n");
    sdCard.CloseFile(file);

    for (uint8_t g = 1; g < countGames; g++) // ignore first, it is themenu
    {
        file = sdCard.CreateJsonFileOnWiki(allGames[g].folder);
        sdCard.FileWriteText(file, "{\n  ");
        std::string temp = JsonTagValue("name", allGames[g].name);
        sdCard.FileWriteText(file, temp.c_str());
        sdCard.FileWriteText(file, "\n");
        switch (allGames[g].machineType)
        {
        case MACHINE_8080BW:
            sdCard.FileWriteText(file, "  \"machine\": \"BW8080\",\n");
            break;
        case MACHINE_Z80:
            sdCard.FileWriteText(file, "  \"machine\": \"Z80\",\n");
            break;
        default:
            sdCard.FileWriteText(file, "  \"machine\": \"???\",\n");
            break;
        }
        sdCard.FileWriteText(file, "  \"rom\": [");
        comma = false;
        uint8_t r = 0;
        std::string romType = "";
        while (true)
        {
            if (allGames[g].machine.roms[r].length == 0 && allGames[g].machine.roms[r].offset == 0)
            {
                break;
            }
            if (allGames[g].machine.roms[r].length == 0)
            {
                switch (allGames[g].machine.roms[r].crc)
                {
                case ROM_CPU:
                    // MY_DEBUG(TAG, "\tROM type: CPU")
                    romType = "CPU";
                    break;
                case ROM_GFX:
                    // MY_DEBUG(TAG, "\tROM type: GFX")
                    romType = "GFX";
                    break;
                case ROM_SOUND:
                    // MY_DEBUG(TAG, "\tROM type: SOUND")
                    romType = "SOUND";
                    break;
                case ROM_COLOR:
                    // MY_DEBUG(TAG, "\tROM type: COLOR")
                    romType = "COLOR";
                    break;
                case ROM_PALETTE:
                    // MY_DEBUG(TAG, "\tROM type: PALETTE")
                    romType = "PALETTE";
                    break;
                default:
                    romType = "UNKNOW ROM TYPE";
                    break;
                }
                r++;
                continue;
            }
            if (comma)
            {
                sdCard.FileWriteText(file, ",");
            }
            else
            {
                comma = true;
            }
            sdCard.FileWriteText(file, "\n");
            sdCard.FileWriteText(file, "    {");
            temp = JsonTagValue("type", romType.c_str());
            sdCard.FileWriteText(file, temp.c_str());
            temp = JsonTagValue("name", allGames[g].machine.roms[r].name);
            sdCard.FileWriteText(file, temp.c_str());
            temp = JsonTagNumber("length", allGames[g].machine.roms[r].length);
            sdCard.FileWriteText(file, temp.c_str());
            temp = JsonTagHexValue("crc", allGames[g].machine.roms[r].crc, false);
            sdCard.FileWriteText(file, temp.c_str());
            sdCard.FileWriteText(file, "}");
            r++;
        }
        sdCard.FileWriteText(file, "\n  ]\n");
        sdCard.FileWriteText(file, "}\n");
        sdCard.CloseFile(file);
    }
}

std::string Export::JsonTagValue(std::string tag, const char *value, bool comma)
{
    std::string res = "\"" + std::string(tag) + "\": \"";
    res.append(value);
    res.append("\"");
    if (comma)
    {
        res.append(", ");
    }
    return res;
}

std::string Export::JsonTagHexValue(std::string tag, unsigned long value, bool comma)
{
    std::string res = "\"" + std::string(tag) + "\": \"";
    res.append(std::to_string(value));
    res.append("\"");
    if (comma)
    {
        res.append(", ");
    }
    return res;
}

std::string Export::JsonTagNumber(std::string tag, unsigned long value, bool comma)
{
    std::string res = "\"" + std::string(tag) + "\": \"";
    res.append(std::to_string(value));
    res.append("\"");
    if (comma)
    {
        res.append(", ");
    }
    return res;
}
