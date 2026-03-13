#include <iostream>
#include <ctime>
#include "sqlite3.h"
#include "accountdatabase.h"
#include "model.h"

/*
typedef struct Account{
    char aName[18]; //卡号
    char aPwd[8]; //密码
    int nStatus; //卡状态
    time_t tStart; //开卡时间
    time_t tEnd; //截止时间
    float fTotalUse; //累计金额
    time_t tLast; // 最后使用时间
    int nUseCount; //使用次数
    float fBalance; //余额
    int nDel; // 删除标记
}Account; //账户类
*/

int searchaccount(char* cardname){
    
}