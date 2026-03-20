#include <iostream>
#include <ctime>
#include "loginoutdata.h"
#include "database.h"
#include "model.hpp"

/*typedef struct LogInfo{
    char aCardName[19]; // 上机卡号
    time_t tStart; // 上机时间
    time_t tEnd; // 下机时间
    float fAmount; // 消费金额
    float fBalance; // 余额
}LogInfo; // 上下机信息*/

sqlitedb logdb(DATA_ROOT"loginout.db");
sqlitedb accountdb(DATA_ROOT"account.db");

int login(const char* cardname){
    init();
}// 上机操作，链接数据库（返回信息：0-有此账号并可以上机，1-账号余额不足，2-账号已经注销，3-账号不能重复上机，4-其他问题退出）

int logout(const char* cardname){
    init();
}// 下机操作，链接数据库（返回信息：0-有此账号并允许下机，1-账号可以下机但欠费，2-账号已经注销，3-账号暂未上机，4-其他问题退出）

void init(){
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    int year = timeinfo->tm_year + 1900;
    
    char tableName[20];
    sprintf(tableName, "loginout%d", year);
    
    const char* columnDefs = "id INTEGER PRIMARY KEY AUTOINCREMENT, aCardName TEXT NOT NULL, tStart INTEGER, tEnd INTEGER, fAmount REAL, fBalance REAL";
    logdb.tablecreate(tableName, columnDefs);
}