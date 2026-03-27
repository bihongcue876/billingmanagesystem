#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"
#include <cstring>
#include <vector>
#include <stdexcept>
#include "model.hpp"

// 默认数据库根目录路径
#define DATA_ROOT "data\\"

// SQLite 回调函数（未使用）
static int callback(void *NotUsed, int argc, char **argv, char **azColName);

// SQLite 数据库操作封装类
class sqlitedb {
public:
    // 构造函数，初始化数据库连接
    sqlitedb(const char* dbPath);

    // 析构函数，关闭数据库连接
    ~sqlitedb();

    // 禁用拷贝构造和拷贝赋值
    sqlitedb(const sqlitedb&) = delete;
    sqlitedb& operator=(const sqlitedb&) = delete;

    // 获取最后一次错误信息
    const char* getLastError() const;

    // 执行不返回数据的 SQL 语句
    bool exec(const char* sql, const std::vector<const char*>& params = {});

    // 执行查询 SQL 语句并返回结果
    std::vector<std::vector<std::string>> query(const char* sql, const std::vector<const char*>& params = {});

    // 创建数据表
    bool tablecreate(const char* tableName, const char* columnDefs);

    // 插入数据
    bool insert(const char* tableName, const std::vector<const char*>& columns, const std::vector<const char*>& values);

    // 更新数据
    bool update(const char* tableName, const char* setClause, const char* whereClause, const std::vector<const char*>& params = {});

    // 删除数据
    bool remove(const char* tableName, const char* whereClause, const std::vector<const char*>& params = {});

private:
    sqlite3* db;
    std::string lastError;

    // 绑定参数到预编译语句
    void bindParams(sqlite3_stmt* stmt, const std::vector<const char*>& params);
};

// 将查询结果转换为 Account 对象
Account queryToAccount(const std::vector<std::vector<std::string>>& result, int index = 0);

#endif // DATABASE_H
