#include <iostream>
#include "../headfile/financeservice.h"

using namespace std;

void topup(void);// 充值
void refund(void);// 退费
void history(void);// 历史
void statistics(void);// 统计

void financemenu(){
    char ch;
    while (true) {
        cout << "\n--------财务服务目录--------" << endl;
        cout << "1. 账户充值\n2. 账户退费\n3. 账户消费记录\n4. 营业额统计\n0. 退出服务\n" << endl;
        cout << "输入数字指令以继续：";
        cin >> ch;
        cin.ignore(1024, '\n'); // 忽略后面的东西
        switch (ch) {
            case '0':
                cout << "正在退出账户服务\n" << endl;
                return;
                break;
            case '1':
                cout << "账户充值\n" << endl;
                topup();
                break;
            case '2':
                cout << "账户退费\n" << endl;
                refund();
                break;
            case '3':
                cout << "账户消费记录\n" << endl;
                history();
                break;
            case '4':
                cout << "营业额统计\n" << endl;
                statistics();
                break;
            default:
                cout << "输入指令不正确，请重新输入\n" << endl;
                break;
        }
    }
}

void topup(void){

}// 充值

void refund(void){

}// 退费

void history(void){

}// 历史

void statistics(void){

}// 统计