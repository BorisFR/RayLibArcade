#ifndef THE_TOOLS_HPP
#define THE_TOOLS_HPP

#pragma once

#include "MyDefine.h"

#include <sys/time.h>
#ifdef ESP32P4
#include "esp_log.h"
#include <esp_timer.h>
#else
#include <stdio.h>
#endif
#include <string.h>

extern struct timeval _startTime;

extern unsigned long millis();

#ifdef __cplusplus
extern "C" {
#endif

//#ifdef ESP32P4
extern void myDebug(const char *tag, const char *text);
extern void myDebug2Text(const char *tag, const char *text, const char *value);
extern void myDebug2(const char *tag, const char *text, int value);
#define MY_DEBUG(tag, text) myDebug(tag, text);
#define MY_DEBUG2TEXT(tag, text, value) myDebug2Text(tag, text, value);
#define MY_DEBUG2(tag, text, value) myDebug2(tag, text, value);
/* #else
extern void myDebug(std::string tag, std::string text);
extern void myDebug2Text(const char *tag, const char *text, const char *value);
extern void myDebug2(std::string tag, std::string text, int value);
#define MY_DEBUG(tag, text) myDebug(tag, text);
#define MY_DEBUG2TEXT(tag, text, value) myDebug2Text(tag, text, value);
#define MY_DEBUG2(tag, text, value) myDebug2(tag, text, value);
#endif
*/

#ifdef __cplusplus
}
#endif

#endif // THE_TOOLS_HPP