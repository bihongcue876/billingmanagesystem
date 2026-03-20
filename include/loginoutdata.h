#ifndef LOGINOUTDATA_H
#define LOGINOUTDATA_H

#include <iostream>
#include "model.hpp"

int login(const char* cardname);// 上机操作，链接数据库
int logout(const char* cardname);// 下机操作，链接数据库

#endif