#include <iostream>
#include <cstring>
#include <ctime>
#include "sqlite3.h"
#include "model.hpp"
#include "accountservice.h"
#include "accountdatabase.h"
#include "utils.hpp"

using namespace std;

void accountsearch(void);
void accountchange(void);
void signupaccount(void);
void deleteaccount(void);
void accountlist(void);  // 查看所有账户

void accountmenu(void){
    char ch;
    while (true) {
        cout << endl;
        cout << "-----------------------------------" << endl;
        cout << "     账户服务目录" << endl;
        cout << "-----------------------------------" << endl;
        cout << "  1. 查询账户" << endl;
        cout << "  2. 账户信息变更" << endl;
        cout << "  3. 注册账户" << endl;
        cout << "  4. 删除账户" << endl;
        cout << "  5. 账户列表" << endl;
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
                cout << "\n>> 查询账户" << endl;
                accountsearch();
                break;
            case '2':
                cout << "\n>> 账户信息变更" << endl;
                accountchange();
                break;
            case '3':
                cout << "\n>> 注册账户" << endl;
                signupaccount();
                break;
            case '4':
                cout << "\n>> 删除账户" << endl;
                deleteaccount();
                break;
            case '5':
                cout << "\n>> 账户列表" << endl;
                accountlist();
                break;
            default:
                cout << "\n输入指令不正确，请重新输入" << endl;
                break;
        }
    }
}

void accountsearch(void){
    cout << "--------卡号查询--------" << endl;
    cout << "请输入需要查询的卡号：";
    char idn[19];
    cin.width(19);
    cin >> idn;
    int status;
    status = searchaccount(idn);
    switch(status){
        case 0:
            cout << "卡信息未找到" << endl;
            break;
        case 1:
            cout << "已找到卡信息" << endl;
            break;
        case 2:
            cout << "账户已删除" << endl;
            break;
        default:
            cout << "出现异常" << endl;
            break;
    }
    cout << "查询完毕" << endl << endl;
}

void accountchange(void){
    cout << "--------信息更改--------" << endl;
    cout << "请输入需要更改的卡号：";
    char idn[19];
    cin.width(19);
    cin >> idn;
    changeaccount(idn);
}

void signupaccount(void){
    cout << "--------注册--------" << endl;
    signup();
}

void deleteaccount(void){
    cout << "--------注销--------" << endl;
    cout << "请输入需要注销的卡号：";
    char idn[19];
    cin.width(19);
    cin >> idn;
    deletecard(idn);
}

void accountlist(void){
    init();
    cout << "--------账户列表--------" << endl;
    
    vector<vector<string>> result = accountdb.query(
        "SELECT aName, nStatus, fBalance, tLast, nUseCount FROM accounts WHERE nDel=0 ORDER BY aName");
    
    if(result.empty()){
        cout << "暂无有效账户" << endl;
        return;
    }
    
    cout << "卡号\t\t状态\t余额\t使用次数\t最后使用时间" << endl;
    cout << "------------------------------------------------------------" << endl;
    
    float totalBalance = 0.0f;
    int count = 0;
    
    for(size_t i = 0; i < result.size(); i++){
        const vector<string>& row = result[i];
        if(row.size() < 5) continue;
        
        string statusStr = "未上机";
        if(row[1] == "1") statusStr = "上机中";
        else if(row[1] == "2") statusStr = "已注销";
        else if(row[1] == "3") statusStr = "失效";
        
        float balance = stof(row[2]);
        int useCount = stoi(row[3]);
        time_t tLast = stol(row[4]);
        
        struct tm* timeinfo = localtime(&tLast);
        char timeStr[32];
        strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %H:%M", timeinfo);
        
        cout << row[0] << "\t" << statusStr << "\t" << balance << "\t" << useCount << "\t\t" << timeStr << endl;
        
        totalBalance += balance;
        count++;
    }
    
    cout << "------------------------------------------------------------" << endl;
    cout << "共计 " << count << " 个有效账户，总余额：" << formatCurrency(totalBalance) << " 元" << endl;
}
