#ifndef MODEL_HPP
#define MODEL_HPP

#include <ctime>
#include <iostream>
#include <string>

enum class UnitType {
    MINUTE,
    HOUR
}; // 计费规则枚举类

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
    std::string sPackageId; // 套餐编号
    UnitType nUnitType; // 计费单位：MINUTE-分钟，HOUR-小时
    float fUnitPrice; // 单价
    int nDel; // 是否失效：0-有效，1-失效
}Billing; // 计费信息

typedef struct LogInfo{
    char aCardName[19]; // 上机卡号
    time_t tStart; // 上机时间
    time_t tEnd; // 下机时间
    float fAmount; // 消费金额
    float fBalance; // 余额
    int nPackageId; // 套餐ID，0表示无套餐
}LogInfo; // 上下机信息


#endif