#include <iostream>
#include <ctime>
#include <string>
#include <cmath>
#include <conio.h>
#include "loginoutdata.h"
#include "database.h"
#include "model.hpp"
#include "accountdatabase.h"
#include "billingdatabase.h"

using namespace std;
/*typedef struct LogInfo{
    char aCardName[19]; // 上机卡号
    time_t tStart; // 上机时间
    time_t tEnd; // 下机时间
    float fAmount; // 消费金额
    float fBalance; // 余额
}LogInfo; // 上下机信息*/

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

static string getLogTableName(){
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char tableName[20];
    sprintf(tableName, "loginout%d", timeinfo->tm_year + 1900);
    return string(tableName);
}

// 上机登录函数
// 返回值: 0-成功, 1-余额不足, 2-卡号已注销或被禁用, 3-卡号已在使用中, 4-其他错误
int login(const char* cardname){
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
        cout << "请输入密码（第" << attempt << "次尝试）：";
        char inputPwd[9];
        int pwdIndex = 0;
        char ch;
        while ((ch = _getch()) != '\r') {
            if (ch == '\b') {
                if (pwdIndex > 0) {
                    pwdIndex--;
                    cout << "\b \b";
                }
            } else if (pwdIndex < 8) {
                inputPwd[pwdIndex] = ch;
                cout << '*';
                pwdIndex++;
            }
        }
        inputPwd[pwdIndex] = '\0';
        cout << endl;
        
        if(strcmp(inputPwd, acc.aPwd) == 0){
            pwdCorrect = true;
            break;
        } else {
            cout << "密码错误！" << endl;
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
    vector<vector<string>> billingResult = billingdb.query("SELECT sPackageId, nUnitType, fUnitPrice FROM billings WHERE nDel=0");
    string packageId;
    
    if(billingResult.empty()){
        cout << "无可用套餐，使用默认套餐" << endl;
        packageId = "0";
    } else {
        cout << "请选择计费套餐：" << endl;
        cout << "编号\t计费单位\t单价" << endl;
        for(size_t i = 0; i < billingResult.size(); i++){
            Billing billing = queryToBilling(billingResult, i);
            string unitTypeStr = (billing.nUnitType == UnitType::MINUTE) ? "分钟" : "小时";
            cout << billing.sPackageId << "\t" << unitTypeStr << "\t" << billing.fUnitPrice << "元" << endl;
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
// 返回值: 0-成功下机, 1-下机成功但欠费, 2-卡号已注销, 3-卡号未上机, 4-其他错误
int logout(const char* cardname){
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
    if(log.nPackageId != 0){
        initbilling();
        string packageIdStr = to_string(log.nPackageId);
        vector<const char*> billingParams = {packageIdStr.c_str()};
        vector<vector<string>> billingResult = billingdb.query("SELECT sPackageId, nUnitType, fUnitPrice FROM billings WHERE sPackageId=? AND nDel=0", billingParams);
        if(!billingResult.empty()){
            Billing billing = queryToBilling(billingResult, 0);
            fUnitPrice = billing.fUnitPrice;
            nUnitType = billing.nUnitType;
        }
    }
    
    float fAmount = calculateFee(log.tStart, now, fUnitPrice, nUnitType);
    float fNewBalance = acc.fBalance - fAmount;
    
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
    
    // 检查是否欠费
    if(fNewBalance < 0){
        return 1;
    }
    
    return 0;
}

// 将查询结果转换为LogInfo结构体
LogInfo queryToLogInfo(const vector<vector<string>>& result, int index){
    LogInfo log;
    if(result.empty() || index >= (int)result.size()){
        memset(&log, 0, sizeof(LogInfo));
        return log;
    }
    const vector<string>& row = result[index];
    if(row.size() < 6){
        memset(&log, 0, sizeof(LogInfo));
        return log;
    }
    strncpy(log.aCardName, row[0].c_str(), 18);
    log.aCardName[18] = '\0';
    log.tStart = stol(row[1]);
    log.tEnd = stol(row[2]);
    log.fAmount = stof(row[3]);
    log.fBalance = stof(row[4]);
    log.nPackageId = row.size() > 5 ? stoi(row[5]) : 0;
    return log;
}

// 初始化登录日志表
static void initLogTable(){
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    int year = timeinfo->tm_year + 1900;
    
    char tableName[20];
    sprintf(tableName, "loginout%d", year);
    
    const char* columnDefs = "id INTEGER PRIMARY KEY AUTOINCREMENT, aCardName TEXT NOT NULL, tStart INTEGER, tEnd INTEGER, fAmount REAL, fBalance REAL, nPackageId INTEGER DEFAULT 0";
    logdb.tablecreate(tableName, columnDefs);
}
