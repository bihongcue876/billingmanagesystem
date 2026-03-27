#include <iostream>
#include <cstring>
#include <ctime>
#include "sqlite3.h"
#include "model.hpp"
#include "accountservice.h"
#include "accountdatabase.h"

using namespace std;

void accountsearch(void);// 查询账户
void accountchange(void);// 账户信息变更
void signupaccount(void);// 注册账户
void deleteaccount(void);// 删除账户

void accountmenu(void){
    char ch;
    while (true) {
        cout << "\n--------账户服务目录--------" << endl;
        cout << "1. 查询账户\n2. 账户信息变更\n3. 注册账户\n4. 删除账户\n0. 退出服务\n" << endl;
        cout << "输入数字指令以继续：";
        cin >> ch;
        cin.ignore(1024, '\n'); // 忽略后面的东西
        switch (ch) {
            case '0':
                cout << "正在退出账户服务\n" << endl;
                return;
                break;
            case '1':
                cout << "进入查询账户\n" << endl;
                accountsearch();
                break;
            case '2':
                cout << "账户信息变更\n" << endl;
                accountchange();
                break;
            case '3':
                cout << "注册账户\n" << endl;
                signupaccount();
                break;
            case '4':
                cout << "删除账户\n" << endl;
                deleteaccount();
                break;
            default:
                cout << "输入指令不正确，请重新输入\n" << endl;
                break;
        }
    }
}

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



void accountsearch(void){
    cout<<"--------卡号查询--------"<<endl;
    cout<<"请输入需要查询的卡号：";
    char idn[19]; // 卡号
    cin.width(19);
    cin>>idn;
    int status;
    status=searchaccount(idn);
    switch(status){
        case 0:
            cout<<"卡信息未找到"<<endl;
            break;
        case 1:
            cout<<"已找到卡信息"<<endl;
            break;
        case 2:
            cout<<"账户已删除"<<endl;
            break;
        default:
            cout<<"出现异常"<<endl;
            break;
    }
    cout<<"查询完毕"<<endl<<endl;
}// 查询账户

void accountchange(void){
    cout<<"--------信息更改--------"<<endl;
    cout<<"请输入需要更改的卡号：";
    char idn[19]; // 卡号
    cin.width(19);
    cin>>idn;
    changeaccount(idn);
}// 账户信息变更

void signupaccount(void){
    cout<<"--------注册--------"<<endl;
    signup();
}// 注册账户

void deleteaccount(void){
    cout<<"--------注销--------"<<endl;
    cout<<"请输入需要注销的卡号：";
    char idn[19]; // 卡号
    cin.width(19);
    cin>>idn;
    deletecard(idn);
}// 注销账户
