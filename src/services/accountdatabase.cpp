#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <cstring>
#include "sqlite3.h"
#include "accountdatabase.h"
#include "model.hpp"
#include "database.h"
#include "utils.hpp"
#include "financedatabase.h"

using namespace std;

// 检查卡号格式：只能包含字母、数字、下划线，长度 1-18 位
bool checkCardNameFormat(const char* cardname){
    if(cardname == nullptr || strlen(cardname) == 0){
        cout << "卡号不能为空！" << endl;
        return false;
    }
    if(strlen(cardname) > 18){
        cout << "卡号长度不能超过 18 位！" << endl;
        return false;
    }
    for(int i = 0; cardname[i] != '\0'; i++){
        char ch = cardname[i];
        // 只允许字母、数字、下划线
        if(!((ch >= '0' && ch <= '9') || 
             (ch >= 'a' && ch <= 'z') || 
             (ch >= 'A' && ch <= 'Z') || 
             ch == '_')){
            cout << "卡号只能包含字母、数字、下划线！" << endl;
            return false;
        }
    }
    return true;
}

sqlitedb accountdb(DATA_ROOT"account.db");

void init(){
    const char* columnDefs = "id INTEGER PRIMARY KEY AUTOINCREMENT, aName TEXT UNIQUE NOT NULL, aPwd TEXT NOT NULL, nStatus INTEGER, tStart INTEGER, tEnd INTEGER, fTotalUse REAL, tLast INTEGER, nUseCount INTEGER, fBalance REAL, nDel INTEGER";
    accountdb.tablecreate("accounts", columnDefs);
}

string formatTime(time_t t) {
    struct tm* timeinfo = localtime(&t);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", timeinfo);
    return string(buffer);
}

int searchaccount(char* cardname){
    // 检查卡号格式
    if(!checkCardNameFormat(cardname)){
        return 0;
    }
    
    init(); // 初始化表格
    vector<const char*> params = {cardname};
    vector<vector<string>> result = accountdb.query("SELECT aName, aPwd, nStatus, tStart, tEnd, fTotalUse, tLast, nUseCount, fBalance, nDel FROM accounts WHERE aName=?", params);
    if(result.empty()){
        return 0;
    }
    Account acc = queryToAccount(result, 0);
    
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
    // 检查卡号格式
    if(!checkCardNameFormat(cardname)){
        return;
    }

    init(); // 初始化表格
    
    // 先查询账户信息
    vector<const char*> checkParams = {cardname};
    vector<vector<string>> checkResult = accountdb.query(
        "SELECT aName, aPwd, nStatus, fBalance, nDel FROM accounts WHERE aName=?", checkParams);
    
    if(checkResult.empty()){
        cout << "账户不存在！" << endl;
        return;
    }
    
    // 检查是否已删除
    if(checkResult[0][4] == "1"){
        cout << "该账户已注销！" << endl;
        return;
    }
    
    cout << "请选择需要修改的部分：\n1.密码\n2.状态" << endl;
    cout << "输入数字标号：";
    int cmd;
    cin >> cmd;
    cin.ignore(1024, '\n');
    
    switch(cmd){
        case 1:
            {
                string newPwd = readPassword("输入新密码：", 8);

                // 只更新密码
                vector<const char*> params = {newPwd.c_str(), cardname};
                if(accountdb.update("accounts", "aPwd=?", "aName=?", params)){
                    cout << "密码修改成功！" << endl;
                } else {
                    cout << "修改失败：" << accountdb.getLastError() << endl;
                }
            }
            break;
        case 2:
            {
                char newStatus[2];
                cout << "修改卡号状态（0-未上机，1-上机中，2-已注销，3-失效）：";
                cin >> newStatus[0];
                cin.ignore(1024, '\n');
                newStatus[1] = '\0';
                
                // 只更新状态
                vector<const char*> params = {newStatus, cardname};
                if(accountdb.update("accounts", "nStatus=?", "aName=?", params)){
                    cout << "状态修改成功！" << endl;
                } else {
                    cout << "修改失败：" << accountdb.getLastError() << endl;
                }
            }
            break;
        default:
            cout << "输入指令错误。" << endl;
            break;
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

    // 检查卡号格式
    if(!checkCardNameFormat(acc.aName)){
        return;
    }

    vector<const char*> checkParams = {acc.aName};
    vector<vector<string>> checkResult = accountdb.query("SELECT aName, nDel FROM accounts WHERE aName=?", checkParams);
    
    if(!checkResult.empty()){
        // 账号存在，检查是否已删除
        // checkResult[0][1] 是 nDel 字段
        if(checkResult[0][1] == "0"){
            // 未删除，不可重复注册
            cout << "账号已存在，不可重复注册！" << endl;
            return;
        } else {
            // 已删除，可以重新启用，初始化账户信息
            cout << "检测到已注销账号，将重新启用并初始化..." << endl;
        }
    }

    string pwd = readPassword("请输入密码（最多8位）：", 8);
    strncpy(acc.aPwd, pwd.c_str(), sizeof(acc.aPwd) - 1);
    acc.aPwd[sizeof(acc.aPwd) - 1] = '\0';
    
    acc.tStart = time(nullptr);
    acc.tEnd = acc.tStart + 365 * 24 * 60 * 60;
    acc.fTotalUse = 0.0f;
    acc.tLast = acc.tStart;
    acc.nUseCount = 0;
    while(true){
        cout << "请输入开卡金额：";
        cin >> acc.fBalance;
        if(cin.fail()){
            cin.clear();
            cin.ignore(1024, '\n');
            cout << "输入格式错误，请重新输入！" << endl;
            continue;
        }
        cin.ignore(1024, '\n');
        if(acc.fBalance <= 0){
            cout << "开卡金额太少，请重新输入！" << endl;
        } else if(acc.fBalance > 1000){
            cout << "开卡金额太多，请重新输入！" << endl;
        } else {
            break;
        }
    }
    acc.nDel = 0;
    acc.nStatus[0] = '0';
    acc.nStatus[1] = '\0';
    // 准备插入数据：先转换数值类型为字符串，再构建参数列表
    string tStartStr   = to_string(acc.tStart);
    string tEndStr     = to_string(acc.tEnd);
    string totalUseStr = to_string(acc.fTotalUse);
    string tLastStr    = to_string(acc.tLast);
    string useCountStr = to_string(acc.nUseCount);
    string balanceStr  = to_string(acc.fBalance);
    string delStr      = to_string(acc.nDel);
    vector<const char*> columns = {"aName", "aPwd", "nStatus", "tStart", "tEnd", "fTotalUse", "tLast", "nUseCount", "fBalance", "nDel"};
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
    
    if(!checkResult.empty()){
        // 已存在的账号，执行更新操作
        vector<const char*> updateParams = {
            acc.aPwd,
            acc.nStatus,
            tStartStr.c_str(),
            tEndStr.c_str(),
            totalUseStr.c_str(),
            tLastStr.c_str(),
            useCountStr.c_str(),
            balanceStr.c_str(),
            delStr.c_str(),
            acc.aName
        };
        if(accountdb.update("accounts", "aPwd=?, nStatus=?, tStart=?, tEnd=?, fTotalUse=?, tLast=?, nUseCount=?, fBalance=?, nDel=?", "aName=?", updateParams)){
            cout << "账号已重新启用，信息已初始化！" << endl;
            // 记录充值交易
            insertTransaction(acc.aName, 1, acc.fBalance, 0.0f, acc.fBalance, "账户充值");
        } else {
            cout << "启用失败：" << accountdb.getLastError() << endl;
        }
    } else {
        // 新账号，执行插入操作
        if(accountdb.insert("accounts", columns, values)){
            cout << "注册成功！" << endl;
            // 记录充值交易
            insertTransaction(acc.aName, 1, acc.fBalance, 0.0f, acc.fBalance, "账户充值");
        } else {
            cout << "注册失败：" << accountdb.getLastError() << endl;
        }
    }
} // 注册

void deletecard(char* cardname){
    // 检查卡号格式
    if(!checkCardNameFormat(cardname)){
        return;
    }
    
    init();

    // 先查询账户信息
    vector<const char*> checkParams = {cardname};
    vector<vector<string>> checkResult = accountdb.query(
        "SELECT aName, fBalance, nDel FROM accounts WHERE aName=?", checkParams);
    
    if(checkResult.empty()){
        cout << "账户不存在！" << endl;
        return;
    }
    
    // 检查是否已删除
    if(checkResult[0][2] == "1"){
        cout << "该账户已注销！" << endl;
        return;
    }
    
    // 获取余额
    float balance = stof(checkResult[0][1]);
    
    // 显示账户信息
    cout << "账户信息：" << endl;
    cout << "  卡号：" << checkResult[0][0] << endl;
    cout << "  余额：" << formatCurrency(balance) << " 元" << endl;
    
    // 检查余额状态
    const float DEBT_THRESHOLD = -100.0f;  // 欠费阈值，超过此值可能是系统错误
    
    if(balance > 0){
        // 有余额，需要先退费
        cout << "提示：该账户还有余额，请先进行退费操作后再注销！" << endl;
        cout << "（如需强制注销并退还余额，请输入 y 确认）" << endl;
        cout << "确定要注销该账户吗？(y/n)：";
        
        char choice;
        cin >> choice;
        cin.ignore(1024, '\n');
        
        if(choice != 'y' && choice != 'Y'){
            cout << "已取消注销" << endl;
            return;
        }
        
        // 执行注销并退还余额
        time_t tEnd = time(nullptr);
        int nDel = 1;
        int nStatus = 2;
        string statusStr = to_string(nStatus);
        string tEndStr   = to_string(tEnd);
        string delStr    = to_string(nDel);
        string zeroBalanceStr = "0";
        
        vector<const char*> params = {
            statusStr.c_str(),
            tEndStr.c_str(),
            delStr.c_str(),
            zeroBalanceStr.c_str(),
            cardname
        };
        
        if(accountdb.update("accounts", "nStatus=?, tEnd=?, nDel=?, fBalance=?", "aName=?", params)){
            insertTransaction(cardname, 2, balance, balance, 0.0f, "账户注销退费");
            cout << "已退还余额：" << formatCurrency(balance) << " 元" << endl;
            cout << "注销成功！" << endl;
        } else {
            cout << "注销失败：" << accountdb.getLastError() << endl;
        }
        
    } else if(balance < 0 && balance >= DEBT_THRESHOLD){
        // 欠费但未超过阈值，需要先还债
        cout << "提示：该账户欠费 " << formatCurrency(-balance) << " 元，请先还清欠款后再注销！" << endl;
        cout << "已取消注销" << endl;
        
    } else if(balance < DEBT_THRESHOLD){
        // 欠费过多，可能是系统错误，允许直接删除
        cout << "警告：该账户欠费严重（" << formatCurrency(balance) << " 元），可能是系统错误。" << endl;
        cout << "确定要强制注销该账户吗？(y/n)：";
        
        char choice;
        cin >> choice;
        cin.ignore(1024, '\n');
        
        if(choice != 'y' && choice != 'Y'){
            cout << "已取消注销" << endl;
            return;
        }
        
        // 执行注销
        time_t tEnd = time(nullptr);
        int nDel = 1;
        int nStatus = 2;
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
            cout << "强制注销成功！（不退还余额）" << endl;
        } else {
            cout << "注销失败：" << accountdb.getLastError() << endl;
        }
        
    } else {
        // 余额为0，可以直接注销
        cout << "确定要注销该账户吗？(y/n)：";
        
        char choice;
        cin >> choice;
        cin.ignore(1024, '\n');
        
        if(choice != 'y' && choice != 'Y'){
            cout << "已取消注销" << endl;
            return;
        }
        
        // 执行注销
        time_t tEnd = time(nullptr);
        int nDel = 1;
        int nStatus = 2;
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
    }
} // 注销