#include <iostream>
#include "billingstandard.h"

using namespace std;

void newstandard(void);// 新增计费标准
void searchstandard(void);// 查询计费标准
void changestandard(void);// 改动计费标准
void deletestandard(void);// 删除计费标准

void billingmenu(){
    char ch;
    while (true) {
        cout << "\n--------计费标准设置目录--------" << endl;
        cout << "1. 新增标准\n2. 查询标准\n3. 修改标准\n4. 删除标准\n0. 退出计费标准设置\n" << endl;
        cout << "输入数字指令以继续：";
        cin >> ch;
        cin.ignore(1024, '\n'); // 忽略后面的东西
        switch (ch) {
            case '0':
                cout << "正在退出计费标准设置\n" << endl;
                return;
                break;
            case '1':
                cout << "新增计费标准\n" << endl;
                newstandard();
                break;
            case '2':
                cout << "查询计费标准\n" << endl;
                searchstandard();
                break;
            case '3':
                cout << "修改计费标准\n" << endl;
                changestandard();
                break;
            case '4':
                cout << "删除计费标准\n" << endl;
                deletestandard();
                break;
            default:
                cout << "输入指令不正确，请重新输入\n" << endl;
                break;
        }
    }
}

void newstandard(void){

}// 新增计费标准

void searchstandard(void){

}// 查询计费标准

void changestandard(void){

}// 改动计费标准

void deletestandard(void){

}// 删除计费标准