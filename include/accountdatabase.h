#include <iostream>
#ifndef ACCOUNTDATABASE_H
#define ACCOUNTDATABASE_H

int searchaccount(char* cardname); // 查询账户
void changeaccount(char* cardname); // 改动账户
void signup(void); // 注册
void deletecard(char* cardname); // 删除

#endif