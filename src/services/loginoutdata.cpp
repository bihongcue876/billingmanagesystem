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

int login(const char* cardname){

}// 上机操作，链接数据库

int logout(const char* cardname){
    
}// 下机操作，链接数据库

void init(){
    const char* columnDefs = "";
    //列定义和写表格（如果没有则建表，如果有则不建表）
    logdb.tablecreate("", columnDefs);
} // 初始化检查