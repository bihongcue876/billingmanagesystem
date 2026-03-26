#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"
#include <cstring>
#include <vector>
#include <stdexcept>
#include "model.hpp"

/**
 * @file database.h
 * @brief SQLite 数据库封装类头文件
 * 
 * 本文件定义了 sqlitedb 类，提供对 SQLite 数据库的封装操作，
 * 包括连接管理、SQL 执行、查询、建表、增删改等功能。
 * 
 * @author 开发者
 * @date 2026-03-26
 */

#define DATA_ROOT "data/"  ///< 默认数据库根目录路径

/**
 * @brief SQLite 回调函数（未使用）
 * @param NotUsed 未使用的参数
 * @param argc 结果列数
 * @param argv 结果数据数组
 * @param azColName 列名数组
 * @return 固定返回 0
 */
static int callback(void *NotUsed, int argc, char **argv, char **azColName);

/**
 * @class sqlitedb
 * @brief SQLite 数据库操作封装类
 * 
 * 提供面向对象的 SQLite 数据库操作接口，支持参数化查询以防止 SQL 注入。
 * 该类禁用拷贝构造和拷贝赋值，确保数据库连接的唯一性。
 */
class sqlitedb {
public:
    /**
     * @brief 构造函数，初始化数据库连接
     * @param dbPath 数据库文件路径（相对于 DATA_ROOT）
     * @throws std::runtime_error 当无法创建目录或连接数据库时抛出
     */
    sqlitedb(const char* dbPath);

    /**
     * @brief 析构函数，关闭数据库连接
     */
    ~sqlitedb();

    // 禁用拷贝构造和拷贝赋值，防止多个对象共享同一数据库连接
    sqlitedb(const sqlitedb&) = delete;              ///< 禁用拷贝构造函数
    sqlitedb& operator=(const sqlitedb&) = delete;   ///< 禁用拷贝赋值运算符

    /**
     * @brief 获取最后一次错误信息
     * @return 错误信息字符串的 C 风格指针
     */
    const char* getLastError() const;

    /**
     * @brief 执行不返回数据的 SQL 语句
     * @param sql SQL 语句字符串
     * @param params 参数列表，用于参数化查询
     * @return 执行成功返回 true，失败返回 false
     */
    bool exec(const char* sql, const std::vector<const char*>& params = {});

    /**
     * @brief 执行查询 SQL 语句并返回结果
     * @param sql SQL 查询语句
     * @param params 参数列表，用于参数化查询
     * @return 二维字符串向量，每行是一个字符串向量
     */
    std::vector<std::vector<std::string>> query(const char* sql, const std::vector<const char*>& params = {});

    /**
     * @brief 创建数据表
     * @param tableName 表名
     * @param columnDefs 列定义字符串（如 "id INTEGER PRIMARY KEY, name TEXT"）
     * @return 创建成功返回 true，失败返回 false
     */
    bool tablecreate(const char* tableName, const char* columnDefs);

    /**
     * @brief 插入数据
     * @param tableName 表名
     * @param columns 列名列表
     * @param values 值列表，与列名一一对应
     * @return 插入成功返回 true，失败返回 false
     */
    bool insert(const char* tableName, const std::vector<const char*>& columns, const std::vector<const char*>& values);

    /**
     * @brief 更新数据
     * @param tableName 表名
     * @param setClause SET 子句（如 "name = ?, age = ?"）
     * @param whereClause WHERE 子句（如 "id = ?"）
     * @param params 参数列表，按顺序填充 SET 和 WHERE 中的占位符
     * @return 更新成功返回 true，失败返回 false
     */
    bool update(const char* tableName, const char* setClause, const char* whereClause, const std::vector<const char*>& params = {});

    /**
     * @brief 删除数据
     * @param tableName 表名
     * @param whereClause WHERE 子句（如 "id = ?"）
     * @param params 参数列表，用于填充 WHERE 中的占位符
     * @return 删除成功返回 true，失败返回 false
     */
    bool remove(const char* tableName, const char* whereClause, const std::vector<const char*>& params = {});

private:
    sqlite3* db;                    ///< SQLite 数据库连接指针
    std::string lastError;          ///< 最后一次错误信息

    /**
     * @brief 绑定参数到预编译语句
     * @param stmt SQLite 预编译语句指针
     * @param params 参数列表
     */
    void bindParams(sqlite3_stmt* stmt, const std::vector<const char*>& params);
};

/**
 * @brief 将查询结果转换为 Account 对象
 * @param result 查询结果（二维字符串向量）
 * @param index 要转换的行索引，默认为 0
 * @return Account 对象，若结果无效则返回零初始化的对象
 */
Account queryToAccount(const std::vector<std::vector<std::string>>& result, int index = 0);

#endif // DATABASE_H
