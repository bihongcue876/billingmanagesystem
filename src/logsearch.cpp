#include <iostream>
#include "headfile/logsearch.h"

using namespace std;

void totalsales(void);// 营业额与日志
void uselogs(void);// 上下机日志
void logprint(void);// 日志打印

void searchmenu(void){
    char ch;
    while (true) {
        cout << "\n--------日志与查询目录--------" << endl;
        cout << "1. 营业额日志\n2. 上下机日志\n3. 日志打印管理\n0. 退出服务\n" << endl;
        cout << "输入数字指令以继续：";
        cin >> ch;
        cin.ignore(1024, '\n'); // 忽略后面的东西
        switch (ch) {
            case '0':
                cout << "正在退出账户服务\n" << endl;
                return;
                break;
            case '1':
                cout << "营业额与日志\n" << endl;
                totalsales();
                break;
            case '2':
                cout << "上下机日志\n" << endl;
                uselogs();
                break;
            case '3':
                cout << "日志打印与管理\n" << endl;
                logprint();
                break;
            default:
                cout << "输入指令不正确，请重新输入\n" << endl;
                break;
        }
    }
}

void totalsales(void){

}// 营业额与日志

void uselogs(void){

}// 上下机日志

void logprint(void){

}// 日志打印