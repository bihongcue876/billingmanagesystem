#ifndef LOGINOUTDATA_H
#define LOGINOUTDATA_H

#include <iostream>
#include <vector>
#include <string>
#include "model.hpp"

int login(const char* cardname);
int logout(const char* cardname);
LogInfo queryToLogInfo(const std::vector<std::vector<std::string>>& result, int index = 0); // 询问转日志结构体

#endif