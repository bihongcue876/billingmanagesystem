#include <iostream>
#include <ctime>
#include <string>
#include <cmath>
#include <algorithm>
#include "loginoutdata.h"
#include "database.h"
#include "model.hpp"
#include "accountdatabase.h"
#include "billingdatabase.h"
#include "utils.hpp"
#include "financedatabase.h"

using namespace std;

sqlitedb logdb(DATA_ROOT"loginout.db");

static void initLogTable();

static string getLogTableName();

static float calculateFee(time_t tStart, time_t tEnd, float fUnitPrice, UnitType nUnitType){
    time_t duration = tEnd - tStart;
    float fAmount;

    if(nUnitType == UnitType::MINUTE){
        float minutes = duration / 60.0f;
        if(minutes < 15){
            minutes = floor(minutes);
        } else if(minutes < 60){
            minutes = ceil(minutes);
        } else {
            minutes = floor(minutes);
        }
        fAmount = minutes * fUnitPrice;
    } else {
        float hours = duration / 3600.0f;
        hours = ceil(hours);
        fAmount = hours * fUnitPrice;
    }
    return fAmount;
}

static float calculateSegmentedFee(time_t tStart, time_t tEnd, Billing billing){
    time_t duration = tEnd - tStart;
    float hours = duration / 3600.0f;
    
    float totalFee = 0.0f;
    float remainingHours = hours;
    
    for(int i = 0; i < billing.nSegmentCount && remainingHours > 0; i++){
        BillingSegment& seg = billing.segments[i];
        
        float segStart = seg.fStartHour;
        float segEnd = (seg.fEndHour == -1) ? remainingHours : seg.fEndHour;
        
        if(hours <= segStart){
            continue;
        }
        
        float billableHours;
        if(segEnd == -1){
            billableHours = hours - segStart;
        } else {
            billableHours = min(segEnd, hours) - segStart;
        }
        
        if(billableHours > 0){
            billableHours = ceil(billableHours);
            totalFee += billableHours * seg.fPricePerHour;
        }
    }
    
    return totalFee;
}

static string getLogTableName(){
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char tableName[20];
    snprintf(tableName, sizeof(tableName), "loginout%d", timeinfo->tm_year + 1900);
    return string(tableName);
}

// 上机登录函数
// 返回值：0-成功，1-余额不足，2-卡号已注销或被禁用，3-卡号已在使用中，4-其他错误
int login(const char* cardname){
    // 检查卡号格式
    if(!checkCardNameFormat(cardname)){
        return 4;
    }
    
    init();
    initLogTable();

    // 查询账户信息
    vector<const char*> params = {cardname};
    vector<vector<string>> result = accountdb.query(
        "SELECT aName, aPwd, nStatus, tStart, tEnd, fTotalUse, tLast, nUseCount, fBalance, nDel FROM accounts WHERE aName=?", params);

    // 账户不存在
    if(result.empty()){
        return 4;
    }

    // 解析账户信息
    Account acc = queryToAccount(result, 0);

    // 检查账户是否已注销或被禁用
    if(acc.nDel == 1 || strcmp(acc.nStatus, "2") == 0){
        return 2;
    }

    // 密码验证：允许输入五次
    bool pwdCorrect = false;
    for(int attempt = 1; attempt <= 5; attempt++){
        string inputPwd = readPassword("请输入密码：", 8);

        if(strcmp(inputPwd.c_str(), acc.aPwd) == 0){
            pwdCorrect = true;
            break;
        } else {
            if(attempt < 5){
                cout << "密码错误，还剩 " << (5 - attempt) << " 次机会" << endl;
            }
        }
    }

    // 五次密码全错，退出函数
    if(!pwdCorrect){
        cout << "密码错误次数过多，上机失败" << endl;
        return 4;
    }

    // 检查账户是否已在上机状态
    if(strcmp(acc.nStatus, "1") == 0){
        return 3;
    }

    // 检查余额是否充足
    if(acc.fBalance <= 0){
        return 1;
    }

    // 显示可用套餐并让管理员选择
    initbilling();
    vector<vector<string>> billingResult = billingdb.query("SELECT sPackageId, sPackageName, nUnitType, fUnitPrice, nSegmentCount FROM billings WHERE nDel=0");
    string packageId;

    if(billingResult.empty()){
        cout << "无可用套餐，使用默认套餐" << endl;
        packageId = "0";
    } else {
        cout << "请选择计费套餐：" << endl;
        cout << "编号\t套餐名称\t\t计费模式\t计费单位\t单价" << endl;
        for(size_t i = 0; i < billingResult.size(); i++){
            Billing billing = queryToBilling(billingResult, i);
            string unitTypeStr = (billing.nUnitType == UnitType::MINUTE) ? "分钟" : "小时";
            string modeStr = (billing.nSegmentCount > 0) ? "分段计费" : "简单计费";
            cout << billing.sPackageId << "\t" << billing.sPackageName << "\t" << modeStr << "\t" << unitTypeStr << "\t" << formatCurrency(billing.fUnitPrice) << "元" << endl;
        }

        while(true){
            cout << "输入套餐编号（直接回车使用默认套餐）：";
            string inputId;
            getline(cin, inputId);

            if(inputId.empty()){
                packageId = "0";
                break;
            }

            bool found = false;
            for(const auto& row : billingResult){
                if(row[0] == inputId){
                    found = true;
                    packageId = inputId;
                    break;
                }
            }

            if(found){
                break;
            } else {
                cout << "套餐编号无效，请重新输入！" << endl;
            }
        }
    }

    // 记录上机日志
    time_t now = time(nullptr);
    string tableName = getLogTableName();
    string tStartStr = to_string(now);
    string fBalanceStr = to_string(acc.fBalance);

    // 插入登录日志记录
    vector<const char*> logColumns = {"aCardName", "tStart", "tEnd", "fAmount", "fBalance", "nPackageId"};
    vector<const char*> logValues = {cardname, tStartStr.c_str(), "-1", "-1", fBalanceStr.c_str(), packageId.c_str()};
    if(!logdb.insert(tableName.c_str(), logColumns, logValues)){
        return 4;
    }

    // 更新账户状态为上机中
    string useCountStr = to_string(acc.nUseCount + 1);
    vector<const char*> updateParams = {"1", tStartStr.c_str(), useCountStr.c_str(), cardname};
    if(!accountdb.update("accounts", "nStatus=?, tLast=?, nUseCount=?", "aName=?", updateParams)){
        return 4;
    }

    return 0;
}

// 下机登出函数
// 返回值：0-成功下机，1-下机成功但欠费，2-卡号已注销，3-卡号未上机，4-其他错误
int logout(const char* cardname){
    // 检查卡号格式
    if(!checkCardNameFormat(cardname)){
        return 4;
    }
    
    init();
    initLogTable();

    // 查询账户信息
    vector<const char*> params = {cardname};
    vector<vector<string>> result = accountdb.query(
        "SELECT aName, aPwd, nStatus, tStart, tEnd, fTotalUse, tLast, nUseCount, fBalance, nDel FROM accounts WHERE aName=?", params);

    // 账户不存在
    if(result.empty()){
        return 4;
    }

    // 解析账户信息
    Account acc = queryToAccount(result, 0);

    // 检查账户是否已注销或被禁用
    if(acc.nDel == 1 || strcmp(acc.nStatus, "2") == 0){
        return 2;
    }

    // 检查账户是否处于未上机状态
    if(strcmp(acc.nStatus, "0") == 0){
        return 3;
    }

    // 查询上机日志记录
    string tableName = getLogTableName();
    vector<const char*> logParams = {cardname};
    string logSql = "SELECT id, aCardName, tStart, tEnd, fAmount, fBalance, nPackageId FROM " + tableName + " WHERE aCardName=? AND tEnd=-1";
    vector<vector<string>> logResult = logdb.query(logSql.c_str(), logParams);

    // 未找到上机记录
    if(logResult.empty()){
        return 3;
    }

    // 计算消费金额
    LogInfo log = queryToLogInfo(logResult, 0);
    time_t now = time(nullptr);

    // 获取套餐计费标准
    float fUnitPrice = 0.1f;
    UnitType nUnitType = UnitType::MINUTE;
    Billing billing;
    billing.nSegmentCount = 0;
    billing.fUnitPrice = 0.1f;
    
    if(log.nPackageId != 0){
        initbilling();
        string packageIdStr = to_string(log.nPackageId);
        vector<const char*> billingParams = {packageIdStr.c_str()};
        vector<vector<string>> billingResult = billingdb.query("SELECT sPackageId, sPackageName, nUnitType, fUnitPrice, nSegmentCount, sSegments, nDel FROM billings WHERE sPackageId=? AND nDel=0", billingParams);
        if(!billingResult.empty()){
            billing = queryToBilling(billingResult, 0);
            fUnitPrice = billing.fUnitPrice;
            nUnitType = billing.nUnitType;
        }
    }

    float fAmount;
    if(billing.nSegmentCount > 0){
        fAmount = calculateSegmentedFee(log.tStart, now, billing);
    } else {
        fAmount = calculateFee(log.tStart, now, fUnitPrice, nUnitType);
    }
    float fNewBalance = round((acc.fBalance - fAmount) * 100.0f) / 100.0f;

    // 更新上机日志记录
    string tEndStr = to_string(now);
    string fAmountStr = to_string(fAmount);
    string fNewBalanceStr = to_string(fNewBalance);
    string idStr = logResult[0][0];

    vector<const char*> updateLogParams = {tEndStr.c_str(), fAmountStr.c_str(), fNewBalanceStr.c_str(), idStr.c_str()};
    if(!logdb.update(tableName.c_str(), "tEnd=?, fAmount=?, fBalance=?", "id=?", updateLogParams)){
        return 4;
    }

    // 更新账户状态为未上机
    string statusStr = "0";
    string tLastStr = to_string(now);
    string balanceStr = to_string(fNewBalance);
    vector<const char*> updateAccParams = {statusStr.c_str(), tLastStr.c_str(), balanceStr.c_str(), cardname};
    if(!accountdb.update("accounts", "nStatus=?, tLast=?, fBalance=?", "aName=?", updateAccParams)){
        return 4;
    }

    // 记录财务交易（消费）
    insertTransaction(cardname, 3, fAmount, acc.fBalance, fNewBalance, "上机消费");

    // 检查是否欠费
    if(fNewBalance < 0){
        return 1;
    }

    return 0;
}

// 将查询结果转换为 LogInfo 结构体
LogInfo queryToLogInfo(const vector<vector<string>>& result, int index){
    LogInfo log;
    if(result.empty() || index >= (int)result.size()){
        memset(&log, 0, sizeof(LogInfo));
        return log;
    }
    const vector<string>& row = result[index];
    if(row.size() < 7){
        memset(&log, 0, sizeof(LogInfo));
        return log;
    }
    // 查询结果包含 id，所以索引从 1 开始
    strncpy(log.aCardName, row[1].c_str(), 18);
    log.aCardName[18] = '\0';

    // 转换数值类型，捕获可能的异常
    try {
        log.tStart = stol(row[2]);
        log.tEnd = stol(row[3]);
        log.fAmount = stof(row[4]);
        log.fBalance = stof(row[5]);
        log.nPackageId = row.size() > 6 ? stoi(row[6]) : 0;
    } catch (const std::exception& e) {
        // 数据格式错误，返回零初始化的对象
        memset(&log, 0, sizeof(LogInfo));
        return log;
    }
    return log;
}

// 初始化登录日志表
static void initLogTable(){
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    int year = timeinfo->tm_year + 1900;

    char tableName[20];
    snprintf(tableName, sizeof(tableName), "loginout%d", year);

    const char* columnDefs = "id INTEGER PRIMARY KEY AUTOINCREMENT, aCardName TEXT NOT NULL, tStart INTEGER, tEnd INTEGER, fAmount REAL, fBalance REAL, nPackageId INTEGER DEFAULT 0";
    logdb.tablecreate(tableName, columnDefs);
}
