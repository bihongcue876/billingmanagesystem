#ifndef MODEL_HPP
#define MODEL_HPP

#include <ctime>
#include <iostream>
#include <string>

/**
 * @file model.hpp
 * @brief 数据结构定义文件
 * 
 * 本文件定义了账户管理系统中使用的所有核心数据结构，
 * 包括账户信息、计费规则、上下机记录和财务交易记录。
 * 
 * @author 开发者
 * @date 2026-03-26
 */

/**
 * @enum UnitType
 * @brief 计费单位类型枚举
 * 
 * 用于指定计费规则的时间单位，支持按分钟或按小时计费。
 */
enum class UnitType {
    MINUTE,  // 按分钟计费
    HOUR     // 按小时计费
};

/**
 * @struct Account
 * @brief 账户信息结构体
 * 
 * 存储用户账户的完整信息，包括卡号、密码、状态、时间信息和余额等。
 */
typedef struct Account {
    char aName[19];      // 卡号，最大 18 个字符
    char aPwd[9];        // 密码，最大 8 个字符
    char nStatus[2];     // 卡状态，如"0"-正常，"1"-冻结等
    time_t tStart;       // 开卡时间，Unix 时间戳
    time_t tEnd;         // 截止时间，Unix 时间戳
    float fTotalUse;     // 累计消费金额
    time_t tLast;        // 最后使用时间，Unix 时间戳
    int nUseCount;       // 使用次数
    float fBalance;      // 当前余额
    int nDel;            // 删除标记，0-有效，1-已删除
} Account;

/**
 * @struct BillingSegment
 * @brief 计费分段结构体
 * 
 * 用于分段计费，每段包含时长范围和单价。
 */
typedef struct BillingSegment {
    float fStartHour;       // 开始小时数（如0表示0小时起）
    float fEndHour;         // 结束小时数（如1表示1小时，-1表示无上限）
    float fPricePerHour;    // 该段的单价（元/小时）
} BillingSegment;

#define MAX_SEGMENTS 10

/**
 * @struct Billing
 * @brief 计费规则结构体
 * 
 * 存储计费套餐的详细信息，包括套餐编号、计费单位和分段计费规则。
 */
typedef struct Billing {
    std::string sPackageId;      // 套餐编号，唯一标识
    std::string sPackageName;    // 套餐名称
    UnitType nUnitType;          // 计费单位：MINUTE-分钟，HOUR-小时
    float fUnitPrice;            // 单价（用于简单计费模式）
    int nSegmentCount;           // 分段数量
    BillingSegment segments[MAX_SEGMENTS]; // 分段计费规则
    int nDel;                   // 是否失效：0-有效，1-失效
} Billing;

/**
 * @struct LogInfo
 * @brief 上下机记录结构体
 * 
 * 存储用户每次上机消费的详细信息，用于日志记录和查询。
 */
typedef struct LogInfo {
    char aCardName[19];  // 上机卡号，最大 18 个字符
    time_t tStart;       // 上机时间，Unix 时间戳
    time_t tEnd;         // 下机时间，Unix 时间戳
    float fAmount;       // 本次消费金额
    float fBalance;      // 消费后余额
    int nPackageId;      // 套餐 ID，0 表示无套餐
} LogInfo;

/**
 * @struct Finance
 * @brief 财务交易记录结构体
 * 
 * 存储所有财务相关的交易记录，包括充值、退费和消费。
 */
typedef struct Finance {
    int id;              // 交易 ID，唯一标识
    std::string cardId;  // 关联的卡号
    int type;            // 交易类型：1-充值，2-退费，3-消费
    float amount;        // 交易金额
    std::string remark;  // 备注信息
    std::string time;    // 交易时间，字符串格式
} Finance;

#endif // MODEL_HPP
