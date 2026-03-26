#include <iostream>
#include "menu.h"
#include "service.h"
using namespace std;

/**
 * @file menu.cpp
 * @brief 菜单实现文件
 * 
 * 本文件实现了主菜单函数，提供用户交互界面，
 * 接收用户输入并调度到相应的服务模块。
 * 
 * @author 开发者
 * @date 2026-03-26
 */

/**
 * @brief 主菜单函数
 * @details 显示主菜单选项，接收用户输入并调用相应的服务函数
 * 
 * 菜单选项包括：
 * - 1. 账户相关：账户管理功能
 * - 2. 上机下机：上下机管理功能
 * - 3. 计费标准设定：计费规则配置
 * - 4. 财务系统：财务交易管理
 * - 5. 查询统计：数据查询和统计
 * - 0. 退出系统
 * 
 * @return 退出码，0 表示正常退出
 */
int menu(void) {
    char ch;  // 存储用户输入的选项
    
    // 主菜单循环
    while (true) {
        // 显示菜单界面
        cout << "--------------------" << endl;
        cout << "--------菜单--------" << endl;
        cout << "1. 账户相关\n2. 上机下机\n3. 计费标准设定\n4. 财务系统\n5. 查询统计\n0. 退出系统\n" << endl;
        cout << "输入数字指令以继续：";
        
        // 读取用户输入
        cin >> ch;
        cin.ignore(1024, '\n');  // 忽略输入缓冲区中的剩余字符，防止影响下次输入
        
        // 根据用户选择执行相应操作
        switch (ch) {
            case '0':
                // 退出系统
                cout << "正在退出系统" << endl;
                return 0;
                
            case '1':
                // 账户服务
                cout << "进入账户服务" << endl;
                service1();
                break;
                
            case '2':
                // 上下机服务
                cout << "进入上下机服务" << endl;
                service2();
                break;
                
            case '3':
                // 计费服务
                cout << "进入计费服务" << endl;
                service3();
                break;
                
            case '4':
                // 财务服务
                cout << "进入财务服务" << endl;
                service4();
                break;
                
            case '5':
                // 查询服务
                cout << "进入查询服务" << endl;
                service5();
                break;
                
            default:
                // 无效输入处理
                cout << "输入指令不正确，请重新输入" << endl;
                break;
        }
    }
}
