#include <iostream>
#include "headfile/accountservice.h"

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

void accountsearch(void){

}// 查询账户

void accountchange(void){

}// 账户信息变更

void signupaccount(void){

}// 注册账户

void deleteaccount(void){

}// 删除账户