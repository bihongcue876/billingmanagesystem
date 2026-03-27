#ifndef SERVICE_H
#define SERVICE_H

/**
 * @file service.h
 * @brief 服务调度接口头文件
 * 
 * 本文件定义了各个服务模块的接口函数，用于调度不同的业务功能模块。
 * 
 * @author 开发者
 * @date 2026-03-26
 */

/**
 * @brief 服务 1：账户管理服务
 * @details 处理账户相关的操作，如开户、销户、修改信息等
 */
void service1();

/**
 * @brief 服务 2：上下机管理服务
 * @details 处理用户上机、下机操作及相关记录
 */
void service2();

/**
 * @brief 服务 3：计费标准设定服务
 * @details 处理计费规则的配置和管理
 */
void service3();

/**
 * @brief 服务 4：财务服务
 * @details 处理财务交易，如充值、退费、账单查询等
 */
void service4();

/**
 * @brief 服务 5：查询统计服务
 * @details 提供各类数据查询和统计功能
 */
void service5();

/**
 * @brief 服务 6：管理员管理服务
 * @details 处理管理员相关的操作，如修改密码、添加/删除管理员等
 */
void service6();

#endif // SERVICE_H
