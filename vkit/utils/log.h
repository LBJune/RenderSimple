
#pragma once
#include <stdio.h>

#define LOG(Severity, format, ...) printf("[%s][%s:%d]: " format "\n", #Severity, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGD(format, ...) LOG(DEBUG, format, __VA_ARGS__)
#define LOGI(format, ...) LOG(INFO, format, __VA_ARGS__)
#define LOGW(format, ...) LOG(WARRING, format,  __VA_ARGS__)
#define LOGE(format, ...) LOG(ERROR,format,  __VA_ARGS__)
#define LOGF(format, ...) LOG(FALT, format, __VA_ARGS__); abort();