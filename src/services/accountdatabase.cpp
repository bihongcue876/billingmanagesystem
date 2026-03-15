#include <iostream>
#include <ctime>
#include "sqlite3.h"
#include "accountdatabase.h"
#include "model.h"
#include "database.h"

using namespace std;

/*
typedef struct Account{
    char aName[18]; //卡号
    char aPwd[8]; //密码
    int nStatus; //卡状态
    time_t tStart; //开卡时间
    time_t tEnd; //截止时间
    float fTotalUse; //累计金额
    time_t tLast; // 最后使用时间
    int nUseCount; //使用次数
    float fBalance; //余额
    int nDel; // 删除标记
}Account; //账户类

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
*/

sqlitedb accountdb("account.db");

int searchaccount(char* cardname){

} // 查找账户

void changeaccount(char* cardname){

} // 改动账户

void signup(void){

} // 注册

void deletecard(char* cardname){

} // 删除