#include <iostream>
#include <ctime>
#include <cstring>
#include "sqlite3.h"
#include "database.h"
#include "model.hpp"

using namespace std;

// ==================== 辅助函数 ====================

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for(int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

// ==================== sqlitedb 类实现 ====================

// sqlitedb 构造函数
sqlitedb::sqlitedb(const char* dbPath) {
    // 先创建 data 目录（如果不存在）
    system("if not exist data\\ mkdir data");

    // 打开数据库连接
    int rc = sqlite3_open(dbPath, &db);
    if(rc) {
        string err = sqlite3_errmsg(db);
        sqlite3_close(db);
        throw runtime_error("无法连接数据库：" + err);
    }
}

// sqlitedb 析构函数
sqlitedb::~sqlitedb() {
    if(db) sqlite3_close(db);
}

// 获取最后一次错误信息
const char* sqlitedb::getLastError() const {
    return lastError.c_str();
}

// 绑定参数到预编译语句
void sqlitedb::bindParams(sqlite3_stmt* stmt, const std::vector<const char*>& params) {
    for (size_t i = 0; i < params.size(); ++i) {
        int idx = static_cast<int>(i + 1);  // SQLite 参数索引从 1 开始
        const char* value = params[i];
        sqlite3_bind_text(stmt, idx, value, -1, SQLITE_TRANSIENT);
    }
}

// 执行不返回数据的 SQL 语句
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

// 执行查询 SQL 语句并返回结果
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

// 创建数据表
bool sqlitedb::tablecreate(const char* tableName, const char* columnDefs) {
    std::string sql = std::string("CREATE TABLE IF NOT EXISTS ") + tableName + " (" + columnDefs + ");";
    return exec(sql.c_str());
}

// 插入数据
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

// 更新数据
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

// 删除数据
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

// 将查询结果转换为 Account 对象
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

    // 转换数值类型，捕获可能的异常
    try {
        acc.tStart = stol(row[3]);      // 开卡时间
        acc.tEnd = stol(row[4]);        // 截止时间
        acc.fTotalUse = stof(row[5]);   // 累计消费
        acc.tLast = stol(row[6]);       // 最后使用时间
        acc.nUseCount = stoi(row[7]);   // 使用次数
        acc.fBalance = stof(row[8]);    // 余额
        acc.nDel = stoi(row[9]);        // 删除标记
    } catch (const std::exception& e) {
        // 数据格式错误，返回零初始化的对象
        // 调用方应检查返回的对象是否有效
        memset(&acc, 0, sizeof(Account));
        return acc;
    }

    return acc;
}
