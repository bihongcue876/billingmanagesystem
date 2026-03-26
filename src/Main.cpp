#include <iostream>

#include "menu.h"

using namespace std;

/**
 * @file Main.cpp
 * @brief 程序入口文件
 * 
 * 本文件包含程序的入口函数 main()，负责初始化系统环境并启动主菜单。
 * 
 * @author 开发者
 * @date 2026-03-26
 */

/**
 * @brief 程序入口函数
 * @details 设置控制台编码为 UTF-8，显示欢迎信息，然后启动主菜单循环
 * @return 程序退出码，0 表示正常退出
 */
int main(void) {
    // 设置 Windows 控制台代码页为 UTF-8，支持中文显示
    system("chcp 65001");
    
    // 显示欢迎信息
    cout << "欢迎访问计费查阅系统" << endl;
    
    // 启动主菜单
    menu();
    
    return 0;
}
