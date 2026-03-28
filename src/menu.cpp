#include <iostream>
#include "menu.h"
#include "service.h"
#include "admin.h"

using namespace std;

int menu(void) {
    char ch;

    while (true) {
        cout << endl;
        cout << "-----------------------------------" << endl;
        cout << "     主菜单" << endl;
        cout << "-----------------------------------" << endl;
        cout << "  当前管理员：" << getCurrentAdmin() << endl;
        cout << "-----------------------------------" << endl;
        cout << "  1. 账户相关" << endl;
        cout << "  2. 上机下机" << endl;
        cout << "  3. 计费标准设定" << endl;
        cout << "  4. 财务系统" << endl;
        cout << "  5. 查询统计" << endl;
        cout << "  6. 管理员管理" << endl;
        cout << "  0. 登出" << endl;
        cout << "-----------------------------------" << endl;
        cout << "请输入选项：";

        cin >> ch;
        cin.ignore(1024, '\n');

        switch (ch) {
            case '0':
                cout << "\n正在登出..." << endl;
                return 0;

            case '1':
                cout << "\n>> 进入账户服务" << endl;
                service1();
                break;

            case '2':
                cout << "\n>> 进入上下机服务" << endl;
                service2();
                break;

            case '3':
                cout << "\n>> 进入计费服务" << endl;
                service3();
                break;

            case '4':
                cout << "\n>> 进入财务服务" << endl;
                service4();
                break;

            case '5':
                cout << "\n>> 进入查询服务" << endl;
                service5();
                break;

            case '6':
                cout << "\n>> 进入管理员管理" << endl;
                service6();
                break;

            default:
                cout << "\n输入指令不正确，请重新输入" << endl;
                break;
        }
    }
}
