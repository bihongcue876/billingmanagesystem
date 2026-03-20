#ifndef MODEL_HPP
#define MODEL_HPP

#include <ctime>
#include <iostream>

typedef struct Account{
    char aName[19]; //卡号
    char aPwd[9]; //密码
    char nStatus[2]; //卡状态
    time_t tStart; //开卡时间
    time_t tEnd; //截止时间
    float fTotalUse; //累计金额
    time_t tLast; // 最后使用时间
    int nUseCount; //使用次数
    float fBalance; //余额
    int nDel; // 删除标记
}Account; //账户类

typedef struct Billing{
    char aCardName[19];
    time_t tStart; // 上机时间
    time_t tEnd; // 下机时间
    float fAmount; // 消费金额
    int nStatus; // 消费状态 0,1
    int nDel; // 删除表示 0,1
}Billing; // 计费信息

typedef struct LogInfo{
    char aCardName[19]; // 上机卡号
    time_t tStart; // 上机时间
    time_t tEnd; // 下机时间
    float fAmount; // 消费金额
    float fBalance; // 余额
}LogInfo; // 上下机信息


#endif