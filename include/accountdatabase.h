#include <iostream>
#ifndef ACCOUNTDATABASE_H
#define ACCOUNTDATABASE_H

#include "database.h"

extern sqlitedb accountdb;

// 检查卡号格式
bool checkCardNameFormat(const char* cardname);

int searchaccount(char* cardname); // 查询账户
void changeaccount(char* cardname); // 改动账户
void signup(void); // 注册
void deletecard(char* cardname); // 删除
void init(void); // 初始化数据库

#endif