#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <conio.h> // 用于 getch() 函数，用于密码保密输入
#include "sqlite3.h"
#include "accountdatabase.h"

#include "model.hpp"
#include "database.h"

using namespace std;

sqlitedb accountdb("account.db");

void init();

string formatTime(time_t t) {
    struct tm* timeinfo = localtime(&t);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", timeinfo);
    return string(buffer);
}

int searchaccount(char* cardname){
    init(); // 初始化表格
    vector<const char*> params = {cardname};
    vector<vector<string>> result = accountdb.query("SELECT aName, aPwd, nStatus, tStart, tEnd, fTotalUse, tLast, nUseCount, fBalance, nDel FROM accounts WHERE aName=?", params);
    if(result.empty()){
        return 0;
    }
    Account acc = queryToAccount(result);
    
    // 格式化时间显示
    string lastTime = formatTime(acc.tLast);
    string startTime = formatTime(acc.tStart);
    
    // 卡状态显示
    string statusStr = "未上机"; // 默认未上机
    if (strcmp(acc.nStatus, "1") == 0) {
        statusStr = "上机中";
    } else if (strcmp(acc.nStatus, "2") == 0) {
        statusStr = "已删除";
    } else if (strcmp(acc.nStatus, "3") == 0) {
        statusStr = "失效";
    }
    
    cout << "账号\t卡状态\t最后使用时间\t使用次数\t余额\t开卡时间" << endl;
    cout << acc.aName << "\t" << statusStr << "\t" << lastTime << "\t" << acc.nUseCount << "\t" << acc.fBalance << "\t" << startTime << endl;
    if(acc.nDel == 1){
        return 2;
    }
    return 1;
} // 

void changeaccount(char* cardname){
    init(); // 初始化表格
    Account acc;
    memset(acc.aPwd, 0, sizeof(acc.aPwd));
    cout<<"请选择需要修改的部分：\n1.密码\n2.状态"<<endl;
    cout<<"输入数字标号：";
    int cmd;
    cin>>cmd;
    cin.ignore(1024,'\n');
    switch(cmd){
        case 1:
            {
            cout<<"输入新密码：";
            int pwdIndex = 0;
            char ch;
            while ((ch = _getch()) != '\r') {
                if (ch == '\b') {
                    if (pwdIndex > 0) {
                        pwdIndex--;
                        cout << "\b \b";
                    }
                } else if (pwdIndex < 8) {
                    acc.aPwd[pwdIndex] = ch;
                    cout << '*';
                    pwdIndex++;
                }
            }
            acc.aPwd[pwdIndex] = '\0';
            cout << endl;
            break;
            }
        case 2:
            cout<<"修改卡号状态（0-未上机，1-上机中，2-已注销，3-失效）：";
            cin>>acc.nStatus[0];
            cin.ignore(1024,'\n');
            acc.nStatus[1]='\0';
            break;
        default:
            cout<<"输入指令错误。"<<endl;
            break;
    }
    vector<const char*> params = {acc.nStatus, acc.aPwd, cardname};
    if(accountdb.update("accounts", "nStatus=?, aPwd=?", "aName=?", params)){
        cout << "修改成功！" << endl;
    } else {
        cout << "修改失败：" << accountdb.getLastError() << endl;
    }
} // 改动卡务信息

void signup(void){
    init(); // 初始化表格
    Account acc;
    memset(acc.aPwd, 0, sizeof(acc.aPwd));
    cout << "请输入卡号（最多18位）：";
    cin.width(19);
    cin >> acc.aName;
    cin.ignore(1024,'\n');
    cout << "请输入密码（最多8位）：";
    int pwdIndex = 0;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (pwdIndex > 0) {
                pwdIndex--;
                cout << "\b \b";
            }
        } else if (pwdIndex < 8) {
            acc.aPwd[pwdIndex] = ch;
            cout << '*';
            pwdIndex++;
        }
    }
    acc.aPwd[pwdIndex] = '\0';
    cout << endl;
    acc.tStart = time(nullptr);
    acc.tEnd = acc.tStart + 365 * 24 * 60 * 60;
    acc.fTotalUse = 0.0f;
    acc.tLast = acc.tStart;
    acc.nUseCount = 0;
    acc.fBalance = 0.0f;
    acc.nDel = 0;
    // 初始化卡状态为未上机
    acc.nStatus[0] = '0';
    acc.nStatus[1] = '\0';
    vector<const char*> columns = {"aName", "aPwd", "nStatus", "tStart", "tEnd", "fTotalUse", "tLast", "nUseCount", "fBalance", "nDel"};
    // 使用局部 string 保存数值型字段，避免悬空指针
    string tStartStr   = to_string(acc.tStart);
    string tEndStr     = to_string(acc.tEnd);
    string totalUseStr = to_string(acc.fTotalUse);
    string tLastStr    = to_string(acc.tLast);
    string useCountStr = to_string(acc.nUseCount);
    string balanceStr  = to_string(acc.fBalance);
    string delStr      = to_string(acc.nDel);
    vector<const char*> values = {
        acc.aName,
        acc.aPwd,
        acc.nStatus,
        tStartStr.c_str(),
        tEndStr.c_str(),
        totalUseStr.c_str(),
        tLastStr.c_str(),
        useCountStr.c_str(),
        balanceStr.c_str(),
        delStr.c_str()
    };
    if(accountdb.insert("accounts", columns, values)){
        cout << "注册成功！" << endl;
    } else {
        cout << "注册失败：" << accountdb.getLastError() << endl;
    }
} // 注册

void deletecard(char* cardname){
    init(); // 初始化表格
    time_t tEnd = time(nullptr);
    int nDel = 1;
    int nStatus = 2;
    // 使用局部 string 保存数值型字段，避免悬空指针
    string statusStr = to_string(nStatus);
    string tEndStr   = to_string(tEnd);
    string delStr    = to_string(nDel);
    vector<const char*> params = {
        statusStr.c_str(),
        tEndStr.c_str(),
        delStr.c_str(),
        cardname
    };
    if(accountdb.update("accounts", "nStatus=?, tEnd=?, nDel=?", "aName=?", params)){
        cout << "注销成功！" << endl;
    } else {
        cout << "注销失败：" << accountdb.getLastError() << endl;
    }
} // 注销

void init(){
    const char* columnDefs = "id INTEGER PRIMARY KEY AUTOINCREMENT, aName TEXT UNIQUE NOT NULL, aPwd TEXT NOT NULL, nStatus INTEGER, tStart INTEGER, tEnd INTEGER, fTotalUse REAL, tLast INTEGER, nUseCount INTEGER, fBalance REAL, nDel INTEGER";
    //列定义和写表格（如果没有则建表，如果有则不建表）
    accountdb.tablecreate("accounts", columnDefs);
} // 初始化检查