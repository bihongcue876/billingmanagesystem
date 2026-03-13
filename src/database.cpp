#include <iostream>
#include <ctime>
#include <cstring>
#include "sqlite3.h"
#include "database.h"


using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   for(int i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
} // 回调


sqlitedb::sqlitedb(const char* dbPath){
    int rc = sqlite3_open(dbPath,&db);
    if(rc){
        string err = sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error("无法连接数据库: " + err);
    }
} //构造函数

sqlitedb::~sqlitedb(){
    if(db) sqlite3_close(db);
} //析构函数

const char* sqlitedb::getLastError() const{
    return lastError.c_str();
} //获得最后一次错误信息

void sqlitedb::bindParams(sqlite3_stmt* stmt, const std::vector<const char*>& params){
    for (size_t i = 0; i < params.size(); ++i) {
        int idx = static_cast<int>(i + 1);
        const char* value = params[i];
        sqlite3_bind_text(stmt, idx, value, -1, SQLITE_TRANSIENT);
    }
} // 绑定列表参数

bool sqlitedb::exec(const char* sql,const std::vector<const char*>& params={}){
    sqlite3_stmt* stmt = nullptr;
    const char* tail=nullptr;
    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,&tail);
}//执行不返回数据的SQL

std::vector<std::vector<std::string>> sqlitedb::query(const char* sql,const std::vector<const char*>& params={}){
    std::vector<std::vector<std::string>> result; // 存储最终结果集的二维向量
    sqlite3_stmt* stmt = nullptr;                  // 预编译语句对象指针
    const char* tail = nullptr;                    // 未使用的 SQL 尾部（忽略）
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
        std::vector<std::string> row;               // 存储当前行的各列值
        for (int i = 0; i < colCount; ++i) {
            // 获取第 i 列的文本值（即使列类型不是 TEXT，也会自动转换）
            const char* val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            if (val) {
                row.push_back(val);                  // 有值则存入
            } else {
                row.push_back("NULL");                // 若为 NULL，存入字符串 "NULL"
            }
        }
        result.push_back(std::move(row));            // 将行添加到结果集（移动语义，避免拷贝）
    }
    // 检查是否正常结束（SQLITE_DONE 表示无更多行）
    if (rc != SQLITE_DONE) {
        lastError = sqlite3_errmsg(db);               // 记录可能的错误
    }
    // 销毁预编译语句，释放资源
    sqlite3_finalize(stmt);
    return result; 
} //查询

bool sqlitedb::tablecreate(const char* tableName, const char* columnDefs){
    std::string sql = std::string("CREATE TABLE IF NOT EXISTS ") + tableName + " (" + columnDefs + ");";
    return exec(sql.c_str());
} // 建表

bool sqlitedb::insert(const char* tableName, const std::vector<const char*>& columns, const std::vector<const char*>& values){
    if (columns.size() != values.size()) {
        lastError = "columns and values size mismatch";
        return false;
    }
    std::string sql = "INSERT INTO ";
    sql += tableName;
    sql += " (";

    for (size_t i = 0; i < columns.size(); ++i) {
        if (i > 0) sql += ", ";
        sql += columns[i];
    }
    sql += ") VALUES (";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) sql += ", ";
        sql += "?";
    }
    sql += ");";

    return exec(sql.c_str(), values);
}//Insert

bool sqlitedb::update(const char* tableName, const char* setClause, const char* whereClause, const std::vector<const char*>& params = {}){
    std::string sql = "UPDATE ";
    sql += tableName;
    sql += " SET ";
    sql += setClause;
    if (whereClause && *whereClause) {
        sql += " WHERE ";
        sql += whereClause;
    }
    sql += ";";
    return exec(sql.c_str(), params);
}// 更改更新

bool sqlitedb::remove(const char* tableName, const char* whereClause, const std::vector<const char*>& params = {}){
    std::string sql = "DELETE FROM ";
    sql += tableName;
    if (whereClause && *whereClause) {
        sql += " WHERE ";
        sql += whereClause;
    }
    sql += ";";
    return exec(sql.c_str(), params);
} // 删除


