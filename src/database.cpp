#include <iostream>
#include <ctime>
#include <cstring>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define access _access
#define mkdir _mkdir
#else
#include <unistd.h>
#endif
#include "sqlite3.h"
#include "database.h"
#include "model.hpp"

using namespace std;

/**
 * @file database.cpp
 * @brief SQLite 数据库封装类实现文件
 * 
 * 本文件实现了 sqlitedb 类的所有成员函数，提供完整的数据库操作功能，
 * 包括连接管理、SQL 执行、参数绑定、查询结果处理等。
 * 
 * @author 开发者
 * @date 2026-03-26
 */

// ==================== 辅助函数 ====================

/**
 * @brief SQLite 回调函数（实际未使用，保留用于兼容）
 * @param NotUsed 未使用的参数
 * @param argc 结果列数
 * @param argv 结果数据数组
 * @param azColName 列名数组
 * @return 固定返回 0
 */
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for(int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

/**
 * @brief 检查目录是否存在
 * @param path 目录路径
 * @return 存在返回 true，不存在返回 false
 */
static bool directoryExists(const char* path) {
#ifdef _WIN32
    struct _stat info;
    if (_stat(path, &info) != 0)
        return false;
    return (info.st_mode & _S_IFDIR) != 0;
#else
    struct stat info;
    if (stat(path, &info) != 0)
        return false;
    return (info.st_mode & S_IFDIR) != 0;
#endif
}

/**
 * @brief 创建目录
 * @param path 目录路径
 * @return 创建成功返回 true，失败返回 false
 */
static bool createDirectory(const char* path) {
#ifdef _WIN32
    return _mkdir(path) == 0;
#else
    return mkdir(path, 0755) == 0;
#endif
}

// ==================== sqlitedb 类实现 ====================

/**
 * @brief 构造函数，初始化数据库连接
 * @details 自动创建数据库文件所在目录（如果不存在）
 * @param dbPath 数据库文件路径（相对于 DATA_ROOT）
 * @throws std::runtime_error 当无法创建目录或连接数据库时抛出
 */
sqlitedb::sqlitedb(const char* dbPath) {
    // 构建完整路径
    string fullPath = string(DATA_ROOT) + dbPath;

    // 提取目录部分并创建（如果不存在）
    size_t pos = fullPath.find_last_of("/\\");
    if (pos != string::npos) {
        string dirPath = fullPath.substr(0, pos);
        if (!directoryExists(dirPath.c_str())) {
            if (!createDirectory(dirPath.c_str())) {
                throw std::runtime_error("无法创建目录：" + dirPath);
            }
        }
    }

    // 打开数据库连接
    int rc = sqlite3_open(fullPath.c_str(), &db);
    if(rc) {
        string err = sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error("无法连接数据库：" + err);
    }
}

/**
 * @brief 析构函数，关闭数据库连接
 */
sqlitedb::~sqlitedb() {
    if(db) sqlite3_close(db);
}

/**
 * @brief 获取最后一次错误信息
 * @return 错误信息字符串的 C 风格指针
 */
const char* sqlitedb::getLastError() const {
    return lastError.c_str();
}

/**
 * @brief 绑定参数到预编译语句
 * @details 将参数列表中的字符串依次绑定到 SQL 语句的占位符（?）上
 * @param stmt SQLite 预编译语句指针
 * @param params 参数列表
 */
void sqlitedb::bindParams(sqlite3_stmt* stmt, const std::vector<const char*>& params) {
    for (size_t i = 0; i < params.size(); ++i) {
        int idx = static_cast<int>(i + 1);  // SQLite 参数索引从 1 开始
        const char* value = params[i];
        sqlite3_bind_text(stmt, idx, value, -1, SQLITE_TRANSIENT);
    }
}

/**
 * @brief 执行不返回数据的 SQL 语句
 * @details 支持 INSERT、UPDATE、DELETE、CREATE 等操作
 * @param sql SQL 语句字符串
 * @param params 参数列表，用于参数化查询
 * @return 执行成功返回 true，失败返回 false
 */
bool sqlitedb::exec(const char* sql, const std::vector<const char*>& params) {
    sqlite3_stmt* stmt = nullptr;
    const char* tail = nullptr;
    
    // 预编译 SQL 语句
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, &tail);
    if (rc != SQLITE_OK) {
        lastError = sqlite3_errmsg(db);
        return false;
    }
    
    // 绑定参数
    bindParams(stmt, params);
    
    // 执行语句
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        lastError = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return false;
    }
    
    // 销毁预编译语句
    sqlite3_finalize(stmt);
    return true;
}

/**
 * @brief 执行查询 SQL 语句并返回结果
 * @details 使用预编译语句和参数绑定，逐行获取查询结果
 * @param sql SQL 查询语句
 * @param params 参数列表，用于参数化查询
 * @return 二维字符串向量，每行是一个字符串向量
 */
std::vector<std::vector<std::string>> sqlitedb::query(const char* sql, const std::vector<const char*>& params) {
    std::vector<std::vector<std::string>> result;  // 存储最终结果集的二维向量
    sqlite3_stmt* stmt = nullptr;                   // 预编译语句对象指针
    const char* tail = nullptr;                     // 未使用的 SQL 尾部（忽略）
    
    // 将 SQL 文本编译为预编译语句对象
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, &tail);
    if (rc != SQLITE_OK) {
        lastError = sqlite3_errmsg(db);  // 记录错误信息
        return result;                    // 返回空结果
    }
    
    // 绑定参数（如果有）
    bindParams(stmt, params);
    
    // 获取结果集的列数
    int colCount = sqlite3_column_count(stmt);
    
    // 逐行获取数据
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::vector<std::string> row;  // 存储当前行的各列值
        for (int i = 0; i < colCount; ++i) {
            // 获取第 i 列的文本值（即使列类型不是 TEXT，也会自动转换）
            const char* val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            if (val) {
                row.push_back(val);       // 有值则存入
            } else {
                row.push_back("NULL");    // 若为 NULL，存入字符串 "NULL"
            }
        }
        result.push_back(std::move(row));  // 将行添加到结果集（移动语义，避免拷贝）
    }
    
    // 检查是否正常结束（SQLITE_DONE 表示无更多行）
    if (rc != SQLITE_DONE) {
        lastError = sqlite3_errmsg(db);  // 记录可能的错误
    }
    
    // 销毁预编译语句，释放资源
    sqlite3_finalize(stmt);
    return result;
}

/**
 * @brief 创建数据表
 * @details 使用 CREATE TABLE IF NOT EXISTS 语句，避免重复创建
 * @param tableName 表名
 * @param columnDefs 列定义字符串
 * @return 创建成功返回 true，失败返回 false
 */
bool sqlitedb::tablecreate(const char* tableName, const char* columnDefs) {
    std::string sql = std::string("CREATE TABLE IF NOT EXISTS ") + tableName + " (" + columnDefs + ");";
    return exec(sql.c_str());
}

/**
 * @brief 插入数据
 * @details 使用参数化查询防止 SQL 注入
 * @param tableName 表名
 * @param columns 列名列表
 * @param values 值列表，与列名一一对应
 * @return 插入成功返回 true，失败返回 false
 */
bool sqlitedb::insert(const char* tableName, const std::vector<const char*>& columns, const std::vector<const char*>& values) {
    // 检查列数和值数是否匹配
    if (columns.size() != values.size()) {
        lastError = "columns and values size mismatch";
        return false;
    }
    
    // 构建 INSERT 语句
    std::string sql = "INSERT INTO ";
    sql += tableName;
    sql += " (";

    // 添加列名
    for (size_t i = 0; i < columns.size(); ++i) {
        if (i > 0) sql += ", ";
        sql += columns[i];
    }
    
    // 添加占位符
    sql += ") VALUES (";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) sql += ", ";
        sql += "?";
    }
    sql += ");";

    // 预编译并执行
    sqlite3_stmt* stmt = nullptr;
    const char* tail = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &tail);
    if (rc != SQLITE_OK) {
        lastError = sqlite3_errmsg(db);
        return false;
    }
    
    bindParams(stmt, values);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        lastError = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return false;
    }
    
    sqlite3_finalize(stmt);
    return true;
}

/**
 * @brief 更新数据
 * @details 构建 UPDATE 语句并执行
 * @param tableName 表名
 * @param setClause SET 子句
 * @param whereClause WHERE 子句
 * @param params 参数列表
 * @return 更新成功返回 true，失败返回 false
 */
bool sqlitedb::update(const char* tableName, const char* setClause, const char* whereClause, const std::vector<const char*>& params) {
    std::string sql = "UPDATE ";
    sql += tableName;
    sql += " SET ";
    sql += setClause;
    
    // 添加 WHERE 子句（如果提供）
    if (whereClause && *whereClause) {
        sql += " WHERE ";
        sql += whereClause;
    }
    sql += ";";
    
    return exec(sql.c_str(), params);
}

/**
 * @brief 删除数据
 * @details 构建 DELETE 语句并执行
 * @param tableName 表名
 * @param whereClause WHERE 子句
 * @param params 参数列表
 * @return 删除成功返回 true，失败返回 false
 */
bool sqlitedb::remove(const char* tableName, const char* whereClause, const std::vector<const char*>& params) {
    std::string sql = "DELETE FROM ";
    sql += tableName;
    
    // 添加 WHERE 子句（如果提供）
    if (whereClause && *whereClause) {
        sql += " WHERE ";
        sql += whereClause;
    }
    sql += ";";
    
    return exec(sql.c_str(), params);
}

// ==================== 辅助函数实现 ====================

/**
 * @brief 将查询结果转换为 Account 对象
 * @details 从查询结果中提取字段并填充 Account 结构体
 * @param result 查询结果（二维字符串向量）
 * @param index 要转换的行索引，默认为 0
 * @return Account 对象，若结果无效则返回零初始化的对象
 */
Account queryToAccount(const vector<vector<string>>& result, int index) {
    Account acc;
    
    // 检查结果有效性
    if(result.empty() || index >= (int)result.size()) {
        memset(&acc, 0, sizeof(Account));
        return acc;
    }
    
    const vector<string>& row = result[index];
    
    // 检查列数是否足够
    if(row.size() < 10) {
        memset(&acc, 0, sizeof(Account));
        return acc;
    }
    
    // 逐字段填充 Account 结构体
    strncpy(acc.aName, row[0].c_str(), 18);
    acc.aName[18] = '\0';
    
    strncpy(acc.aPwd, row[1].c_str(), 8);
    acc.aPwd[8] = '\0';
    
    strncpy(acc.nStatus, row[2].c_str(), 1);
    acc.nStatus[1] = '\0';
    
    acc.tStart = stol(row[3]);      // 开卡时间
    acc.tEnd = stol(row[4]);        // 截止时间
    acc.fTotalUse = stof(row[5]);   // 累计消费
    acc.tLast = stol(row[6]);       // 最后使用时间
    acc.nUseCount = stoi(row[7]);   // 使用次数
    acc.fBalance = stof(row[8]);    // 余额
    acc.nDel = stoi(row[9]);        // 删除标记
    
    return acc;
}
