#include <iostream>
#include "service.h"
#include "accountservice.h"
#include "billingstandard.h"
#include "financeservice.h"
#include "loginout.h"
#include "logsearch.h"
#include "adminservice.h"

using namespace std;

/**
 * @file service.cpp
 * @brief 服务调度实现文件
 * 
 * 本文件实现了各个服务模块的调度函数，负责调用相应的子菜单函数。
 * 
 * @author 开发者
 * @date 2026-03-26
 */

/**
 * @brief 服务 1：账户管理服务调度
 * @details 调用账户管理子菜单
 */
void service1() {
    accountmenu();  // 进入账户管理子菜单
}

/**
 * @brief 服务 2：上下机管理服务调度
 * @details 调用上下机管理子菜单
 */
void service2() {
    logmenu();  // 进入上下机管理子菜单
}

/**
 * @brief 服务 3：计费标准设定服务调度
 * @details 调用计费标准管理子菜单
 */
void service3() {
    billingmenu();  // 进入计费标准管理子菜单
}

/**
 * @brief 服务 4：财务服务调度
 * @details 调用财务管理子菜单
 */
void service4() {
    financemenu();  // 进入财务管理子菜单
}

void service5() {
    searchmenu();
}

// 服务 6：管理员管理服务调度
void service6() {
    adminmenu();
}
