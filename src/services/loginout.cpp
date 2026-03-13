#include <iostream>
#include "loginout.h"

using namespace std;

void login(void);
void logout(void);

void logmenu(void){
    char ch;
    while (true) {
        cout << "\n--------上下机服务目录--------" << endl;
        cout << "1. 用户上机\n2. 用户下机\n0. 退出上下机服务\n" << endl;
        cout << "输入数字指令以继续：";
        cin >> ch;
        cin.ignore(1024, '\n'); // 忽略后面的东西
        switch (ch) {
            case '0':
                cout << "正在退出上下机服务" << endl;
                return;
                break;
            case '1':
                cout << "用户上机\n" << endl;
                login();
                break;
            case '2':
                cout << "用户下机\n" << endl;
                logout();
                break;
            default:
                cout << "输入指令不正确，请重新输入\n" << endl;
                break;
        }
    }
}

void login(void){

}// 上机

void logout(void){

}// 下机
