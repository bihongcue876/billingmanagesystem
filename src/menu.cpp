#include <iostream>
#include "headfile/menu.h"
#include "headfile/service.h"
using namespace std;

int menu(void) {
    char ch;
    while (true) {
        cout << "--------------------" << endl;
        cout << "--------菜单--------" << endl;
        cout << "1. 账户相关\n2. 上机下机\n3. 计费标准设定\n4. 财务系统\n5. 查询统计\n0. 退出系统\n" << endl;
        cout << "输入数字指令以继续：";
        cin >> ch;
        cin.ignore(1024, '\n'); // 忽略后面的东西
        switch (ch) {
            case '0':
                cout << "正在退出系统" << endl;
                return 0;
                break;
            case '1':
                cout << "进入账户服务" << endl;
                service1();
                break;
            case '2':
                cout << "进入上下机服务" << endl;
                service2();
                break;
            case '3':
                cout << "进入计费服务" << endl;
                service3();
                break;
            case '4':
                cout << "进入财务服务" << endl;
                service4();
                break;
            case '5':
                cout << "进入查询服务" << endl;
                service5();
                break;
            default:
                cout << "输入指令不正确，请重新输入" << endl;
                break;
        }
    }
}