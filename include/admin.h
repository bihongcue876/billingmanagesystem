#ifndef ADMIN_H
#define ADMIN_H

#include <string>

// 初始化管理员表（如果不存在则创建默认管理员）
void initAdminTable();

// 管理员登录，返回 1 成功，0 失败/退出
int admin();

// 获取当前登录的管理员
std::string getCurrentAdmin();

#endif
