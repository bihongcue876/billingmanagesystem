#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"
#include <cstring>
#include <vector>
#include <stdexcept>
#define DATA_ROOT "../data/" //默认数据库地址

static int callback(void *NotUsed, int argc, char **argv, char **azColName);

class sqlitedb{
public:
    sqlitedb(const char* dbPath); //构造函数
    ~sqlitedb();
    sqlitedb(const sqlitedb&) = delete; //禁用拷贝
    sqlitedb& operator=(const sqlitedb&) = delete; //禁用拷贝
    const char* getLastError() const; //获得最后一次错误信息
    bool exec(const char* sql,const std::vector<const char*>& params={});//执行不返回数据的SQL
    std::vector<std::vector<std::string>> query(const char* sql,const std::vector<const char*>& params={}); //查询
    bool tablecreate(const char* tableName, const char* columnDefs); // 建表
    bool insert(const char* tableName, const std::vector<const char*>& columns, const std::vector<const char*>& values);//Insert
    bool update(const char* tableName, const char* setClause, const char* whereClause, const std::vector<const char*>& params = {});// 更改更新
    bool remove(const char* tableName, const char* whereClause, const std::vector<const char*>& params = {}); // 删除
private:
    sqlite3* db;
    std::string lastError;

    void bindParams(sqlite3_stmt* stmt, const std::vector<const char*>& params);//绑定参数，参数列表const char*的vector
};

#endif // DATABASE_H