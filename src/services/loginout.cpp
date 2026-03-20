#include <iostream>
#include "loginout.h"
#include "loginoutdata.h"

using namespace std;

void alogin(void);
void alogout(void);

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
                alogin();
                break;
            case '2':
                cout << "用户下机\n" << endl;
                alogout();
                break;
            default:
                cout << "输入指令不正确，请重新输入\n" << endl;
                break;
        }
    }
}

void alogin(void){
    
}// 上机

void alogout(void){

}// 下机
