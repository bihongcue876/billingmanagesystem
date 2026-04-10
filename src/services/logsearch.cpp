#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <conio.h>
#include "logsearch.h"
#include "financedatabase.h"
#include "loginoutdata.h"
#include "database.h"
#include "model.hpp"

using namespace std;

// 声明外部的 logdb 数据库对象
extern sqlitedb logdb;

void totalsales(void);
void uselogs(void);
void logprint(void);
void logsByDate(void);  // 按日期查询日志

// 获取当前年份的上下机日志表名
static string getLogTableName(){
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char tableName[20];
    snprintf(tableName, sizeof(tableName), "loginout%d", timeinfo->tm_year + 1900);
    return string(tableName);
}

void searchmenu(void){
    char ch;
    while (true) {
        cout << endl;
        cout << "-----------------------------------" << endl;
        cout << "     查询统计菜单" << endl;
        cout << "-----------------------------------" << endl;
        cout << "  1. 营业额日志" << endl;
        cout << "  2. 上下机日志" << endl;
        cout << "  3. 日志打印管理" << endl;
        cout << "  4. 按日期查询" << endl;
        cout << "  0. 返回主菜单" << endl;
        cout << "-----------------------------------" << endl;
        cout << "请输入选项：";
        cin >> ch;
        cin.ignore(1024, '\n');
        switch (ch) {
            case '0':
                cout << "\n正在返回主菜单..." << endl;
                return;
            case '1':
                cout << "\n>> 营业额日志" << endl;
                totalsales();
                break;
            case '2':
                cout << "\n>> 上下机日志" << endl;
                uselogs();
                break;
            case '3':
                cout << "\n>> 日志打印管理" << endl;
                logprint();
                break;
            case '4':
                cout << "\n>> 按日期查询" << endl;
                logsByDate();
                break;
            default:
                cout << "\n输入指令不正确，请重新输入" << endl;
                break;
        }
    }
}

void logsByDate(void);  // 按日期查询日志

// 营业额与日志 - 显示所有财务交易记录并统计
void totalsales(void){
    initfinance();
    
    cout << "=== 营业额统计 ===" << endl;
    
    // 查询所有交易记录
    vector<Finance> allTransactions = queryAllTransactions();
    
    if(allTransactions.empty()){
        cout << "暂无交易记录" << endl;
        return;
    }
    
    // 分类统计
    float totalRecharge = 0.0f;    // 充值总额
    float totalRefund = 0.0f;      // 退费总额
    float totalConsumption = 0.0f; // 消费总额
    
    cout << "ID\t卡号\t\t类型\t金额\t\t备注\t\t时间" << endl;
    cout << "------------------------------------------------------------------------" << endl;
    for(const auto& trans : allTransactions){
        string typeStr;
        if(trans.type == 1) {
            typeStr = "充值";
            totalRecharge += trans.amount;
        } else if(trans.type == 2) {
            typeStr = "退费";
            totalRefund += trans.amount;
        } else if(trans.type == 3) {
            typeStr = "消费";
            totalConsumption += trans.amount;
        }
        cout << trans.id << "\t" << trans.cardId << "\t" << typeStr << "\t" 
             << trans.amount << "\t" << trans.remark << "\t" << trans.time << endl;
    }
    
    cout << "------------------------------------------------------------------------" << endl;
    cout << "\n=== 统计汇总 ===" << endl;
    cout << "充值总额：" << totalRecharge << " 元" << endl;
    cout << "退费总额：" << totalRefund << " 元" << endl;
    cout << "消费总额：" << totalConsumption << " 元" << endl;
    cout << "实际营收：" << (totalRecharge - totalRefund) << " 元" << endl;
}

// 上下机日志 - 显示所有上下机记录
void uselogs(void){
    cout << "=== 上下机日志 ===" << endl;
    
    // 初始化日志表
    extern sqlitedb logdb;
    string tableName = getLogTableName();
    
    // 查询所有日志记录（按结束时间倒序）
    string sql = "SELECT id, aCardName, tStart, tEnd, fAmount, fBalance, nPackageId FROM " + tableName + " WHERE tEnd != -1 ORDER BY tEnd DESC";
    vector<vector<string>> result = logdb.query(sql.c_str());
    
    if(result.empty()){
        cout << "暂无上下机记录" << endl;
        return;
    }
    
    cout << "ID\t卡号\t\t上机时间\t\t下机时间\t\t消费金额\t余额\t套餐" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    
    for(size_t i = 0; i < result.size(); i++){
        const vector<string>& row = result[i];
        if(row.size() < 7) continue;
        
        // 转换时间戳
        time_t tStart = stol(row[2]);
        time_t tEnd = stol(row[3]);
        
        struct tm* startInfo = localtime(&tStart);
        struct tm* endInfo = localtime(&tEnd);
        
        char startTime[32], endTime[32];
        strftime(startTime, sizeof(startTime), "%Y/%m/%d %H:%M:%S", startInfo);
        strftime(endTime, sizeof(endTime), "%Y/%m/%d %H:%M:%S", endInfo);
        
        string packageStr = (row[6] == "0") ? "无" : row[6];
        
        cout << row[0] << "\t" << row[1] << "\t" << startTime << "\t" << endTime << "\t" 
             << row[4] << "\t" << row[5] << "\t" << packageStr << endl;
    }
}

// 日志打印管理 - 按卡号查询并打印日志
void logprint(void){
    cout << "=== 日志打印管理 ===" << endl;
    
    char cardname[19];
    cout << "请输入卡号（直接回车查询所有）：";
    cin.getline(cardname, sizeof(cardname));
    
    if(strlen(cardname) == 0){
        // 查询所有记录
        uselogs();
        return;
    }
    
    // 查询指定卡号的记录
    extern sqlitedb logdb;
    string tableName = getLogTableName();
    
    vector<const char*> params = {cardname};
    string sql = "SELECT id, aCardName, tStart, tEnd, fAmount, fBalance, nPackageId FROM " + tableName + " WHERE aCardName=? AND tEnd != -1 ORDER BY tEnd DESC";
    vector<vector<string>> result = logdb.query(sql.c_str(), params);
    
    if(result.empty()){
        cout << "未找到卡号 [" << cardname << "] 的上下机记录" << endl;
        return;
    }
    
    cout << "\n=== 卡号：" << cardname << " 的上下机记录 ===" << endl;
    cout << "ID\t上机时间\t\t下机时间\t\t消费金额\t余额\t套餐" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    
    float totalAmount = 0.0f;
    int count = 0;
    
    for(size_t i = 0; i < result.size(); i++){
        const vector<string>& row = result[i];
        if(row.size() < 7) continue;
        
        // 转换时间戳
        time_t tStart = stol(row[2]);
        time_t tEnd = stol(row[3]);
        
        struct tm* startInfo = localtime(&tStart);
        struct tm* endInfo = localtime(&tEnd);
        
        char startTime[32], endTime[32];
        strftime(startTime, sizeof(startTime), "%Y/%m/%d %H:%M:%S", startInfo);
        strftime(endTime, sizeof(endTime), "%Y/%m/%d %H:%M:%S", endInfo);
        
        string packageStr = (row[6] == "0") ? "无" : row[6];
        float amount = stof(row[4]);
        totalAmount += amount;
        count++;
        
        cout << row[0] << "\t" << startTime << "\t" << endTime << "\t" 
             << amount << "\t" << row[5] << "\t" << packageStr << endl;
    }
    
    cout << "--------------------------------------------------------------------------------" << endl;
    cout << "共计 " << count << " 条记录，总消费：" << totalAmount << " 元" << endl;

    // 询问是否打印（模拟）
    cout << "\n是否打印以上记录？(y/n)：";
    char choice;
    cin >> choice;
    if(choice == 'y' || choice == 'Y'){
        cout << "正在打印..." << endl;
        cout << "[打印功能已模拟完成]" << endl;
    }
}

// 按日期查询上下机日志
void logsByDate(void){
    cout << "=== 按日期查询上下机日志 ===" << endl;

    string startDate, endDate;
    cout << "请输入开始日期（格式：YYYY-MM-DD）：";
    getline(cin, startDate);
    cout << "请输入结束日期（格式：YYYY-MM-DD）：";
    getline(cin, endDate);

    if(startDate.empty() || endDate.empty()){
        cout << "日期不能为空！" << endl;
        return;
    }

    // 验证日期格式
    int startYear, startMonth, startDay;
    int endYear, endMonth, endDay;

    if(sscanf(startDate.c_str(), "%d-%d-%d", &startYear, &startMonth, &startDay) != 3){
        cout << "开始日期格式错误！正确格式：YYYY-MM-DD" << endl;
        return;
    }
    if(sscanf(endDate.c_str(), "%d-%d-%d", &endYear, &endMonth, &endDay) != 3){
        cout << "结束日期格式错误！正确格式：YYYY-MM-DD" << endl;
        return;
    }

    // 验证日期合法性
    if(startYear < 2000 || startYear > 2100 || startMonth < 1 || startMonth > 12 || startDay < 1 || startDay > 31){
        cout << "开始日期不合法！" << endl;
        return;
    }
    if(endYear < 2000 || endYear > 2100 || endMonth < 1 || endMonth > 12 || endDay < 1 || endDay > 31){
        cout << "结束日期不合法！" << endl;
        return;
    }

    extern sqlitedb logdb;
    string tableName = getLogTableName();

    struct tm startTime = {};
    struct tm endTime = {};
    startTime.tm_year = startYear - 1900;
    startTime.tm_mon = startMonth - 1;
    startTime.tm_mday = startDay;
    startTime.tm_hour = 0;
    startTime.tm_min = 0;
    startTime.tm_sec = 0;

    endTime.tm_year = endYear - 1900;
    endTime.tm_mon = endMonth - 1;
    endTime.tm_mday = endDay;
    endTime.tm_hour = 23;
    endTime.tm_min = 59;
    endTime.tm_sec = 59;

    time_t startTimestamp = mktime(&startTime);
    time_t endTimestamp = mktime(&endTime);

    if(startTimestamp == -1 || endTimestamp == -1){
        cout << "日期转换失败！" << endl;
        return;
    }

    if(startTimestamp > endTimestamp){
        cout << "开始日期不能晚于结束日期！" << endl;
        return;
    }

    // 查询指定日期范围的日志记录
    string sql = "SELECT id, aCardName, tStart, tEnd, fAmount, fBalance, nPackageId FROM " +
                 tableName + " WHERE tStart >= ? AND tStart <= ? AND tEnd != -1 ORDER BY tStart DESC";

    string startStr = to_string(startTimestamp);
    string endStr = to_string(endTimestamp);
    vector<const char*> params = {startStr.c_str(), endStr.c_str()};
    vector<vector<string>> result = logdb.query(sql.c_str(), params);

    if(result.empty()){
        cout << "未找到指定日期范围的上下机记录！" << endl;
        return;
    }

    cout << "ID\t卡号\t上机时间\t\t下机时间\t\t消费金额\t余额\t套餐" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    float totalAmount = 0.0f;
    int count = 0;

    for(size_t i = 0; i < result.size(); i++){
        const vector<string>& row = result[i];
        if(row.size() < 7) continue;

        time_t tStart = stol(row[2]);
        time_t tEnd = stol(row[3]);

        struct tm* startInfo = localtime(&tStart);
        struct tm* endInfo = localtime(&tEnd);

        char startTimeStr[32], endTimeStr[32];
        strftime(startTimeStr, sizeof(startTimeStr), "%Y/%m/%d %H:%M:%S", startInfo);
        strftime(endTimeStr, sizeof(endTimeStr), "%Y/%m/%d %H:%M:%S", endInfo);

        string packageStr = (row[6] == "0") ? "无" : row[6];
        float amount = stof(row[4]);
        totalAmount += amount;
        count++;

        cout << row[0] << "\t" << row[1] << "\t" << startTimeStr << "\t" << endTimeStr << "\t"
             << amount << "\t" << row[5] << "\t" << packageStr << endl;
    }

    cout << "--------------------------------------------------------------------------------" << endl;
    cout << "共计 " << count << " 条记录，总消费：" << totalAmount << " 元" << endl;
}
