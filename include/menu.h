#ifndef MENU_H
#define MENU_H

/**
 * @file menu.h
 * @brief 菜单接口头文件
 * 
 * 本文件定义了菜单系统的接口函数，提供用户交互界面的入口。
 * 
 * @author 开发者
 * @date 2026-03-26
 */

/**
 * @brief 主菜单函数
 * @details 显示主菜单界面，接收用户输入并调度到相应的服务模块
 * @return 退出码，0 表示正常退出
 */
int menu(void);

#endif // MENU_H
