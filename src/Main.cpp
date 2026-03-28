#include <iostream>
#include "menu.h"
#include "admin.h"

using namespace std;

int main(void) {
    system("chcp 65001");
    
    while (true) {
        cout << endl;
        cout << "---------------------------" << endl;
        cout << "     计费管理系统" << endl;
        cout << "---------------------------" << endl;
        cout << "  1. 登录" << endl;
        cout << "  0. 退出系统" << endl;
        cout << "---------------------------" << endl;
        cout << "请输入选项：";
        
        char choice;
        cin >> choice;
        cin.ignore(1024, '\n');
        
        if (choice == '1') {
            int status = admin();
            if (!status) {
                cout << "\n登录失败，返回主菜单" << endl;
                continue;
            }
            menu();
            cout << "\n已登出，请重新登录或退出系统" << endl;
        } else if (choice == '0') {
            cout << "\n正在退出系统..." << endl;
            return 0;
        } else {
            cout << "\n输入指令不正确，请重新输入" << endl;
        }
    }
    
    return 0;
}
