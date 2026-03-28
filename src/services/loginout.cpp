#include <iostream>
#include "loginout.h"
#include "loginoutdata.h"

using namespace std;

void alogin(void);
void alogout(void);

void logmenu(void){
    char ch;
    while (true) {
        cout << endl;
        cout << "-----------------------------------" << endl;
        cout << "     上下机服务目录" << endl;
        cout << "-----------------------------------" << endl;
        cout << "  1. 用户上机" << endl;
        cout << "  2. 用户下机" << endl;
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
                cout << "\n>> 用户上机" << endl;
                alogin();
                break;
            case '2':
                cout << "\n>> 用户下机" << endl;
                alogout();
                break;
            default:
                cout << "\n输入指令不正确，请重新输入" << endl;
                break;
        }
    }
}

void alogin(void){
    cout << "--------用户上机--------" << endl;
    cout << "请输入卡号：";
    char idn[19];
    cin.width(19);
    cin >> idn;
    cin.ignore(1024, '\n');

    int status = login(idn);
    switch(status){
        case 0:
            cout << "上机成功" << endl;
            break;
        case 1:
            cout << "余额不足，无法上机" << endl;
            break;
        case 2:
            cout << "卡号已注销或被禁用" << endl;
            break;
        case 3:
            cout << "卡号已在使用中" << endl;
            break;
        case 4:
            cout << "卡号不存在或密码错误次数过多" << endl;
            break;
        default:
            cout << "出现异常" << endl;
            break;
    }
    cout << idn << "上机操作完毕" << endl << endl;
}// 上机

void alogout(void){
    cout << "--------用户下机--------" << endl;
    cout << "请输入卡号：";
    char idn[19];
    cin.width(19);
    cin >> idn;
    cin.ignore(1024, '\n');

    int status = logout(idn);
    switch(status){
        case 0:
            cout << "下机成功" << endl;
            break;
        case 1:
            cout << "下机成功，但已欠费" << endl;
            break;
        case 2:
            cout << "卡号已注销" << endl;
            break;
        case 3:
            cout << "卡号未上机" << endl;
            break;
        case 4:
            cout << "卡号不存在或其他错误" << endl;
            break;
        default:
            cout << "出现异常" << endl;
            break;
    }
    cout << idn << "下机操作完毕" << endl << endl;
}// 下机
