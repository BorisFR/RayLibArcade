#include "TheTools.hpp"
#ifdef ESP32P4
#include <cstdio>
#else
#include <thread>
#include <chrono>
#endif

struct timeval _startTime;

#ifdef ESP32P4
unsigned long millis()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return (unsigned long)(((currentTime.tv_sec - _startTime.tv_sec) * 1000) + ((currentTime.tv_usec - _startTime.tv_usec) / 1000)); // - _timeSuspended;
}
#else
unsigned long millis()
{
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return duration.count();
}
#endif

//#ifdef ESP32P4
void myDebug(const char *tag, const char *text)
{
    printf("[%s] %s\n", tag, text);
    fflush(stdout);
    // ESP_LOGI(tag, text);
}
void myDebug2Text(const char *tag, const char *text, const char *value)
{
    printf("[%s] %s %s\n", tag, text, value);
    fflush(stdout);
}
void myDebug2(const char *tag, const char *text, int value)
{
    printf("[%s] %s %i\n", tag, text, value);
    fflush(stdout);
}
/*#else
void myDebug(std::string tag, std::string text)
{
    Serial.println("[" + tag + "] " + text);
}
//void myDebug2Text(std::string tag, std::string text, std::string text2)
//{
//    Serial.println("[" + tag + "] " + text + " " + text2);
//}
void myDebug2Text(const char *tag, const char *text, const char *value)
{
    //Serial.println("[" + tag + "] " + text + " " + text2);
    printf("[%s] %s %s\n", tag, text, value);
}
void myDebug2(std::string tag, std::string text, int value)
{
    Serial.println("[" + tag + "] " + text + " " + std::to_string(value));
}
#endif
*/