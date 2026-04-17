#include <iostream>
#include <string>
#include <vector>
#include "admin.h"
#include "database.h"
#include "utils.hpp"

using namespace std;

// 当前登录的管理员
string g_currentAdmin = "";

// 获取当前登录的管理员
string getCurrentAdmin(){
    return g_currentAdmin;
}

// 初始化管理员表
void initAdminTable(){
    sqlitedb admindb(DATA_ROOT"admin.db");
    admindb.tablecreate("admins", "id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE NOT NULL, password TEXT NOT NULL, createTime TEXT");
    
    // 检查是否有管理员，没有则创建默认的
    vector<vector<string>> result = admindb.query("SELECT COUNT(*) FROM admins");
    if(result.empty() || result[0][0] == "0"){
        // 创建默认管理员：admin / 123456
        vector<const char*> columns = {"username", "password"};
        vector<const char*> values = {"admin", "123456"};
        admindb.insert("admins", columns, values);
    }
}

// 验证管理员账号密码
static bool verifyAdmin(const string& username, const string& password){
    sqlitedb admindb(DATA_ROOT"admin.db");
    vector<const char*> params = {username.c_str(), password.c_str()};
    vector<vector<string>> result = admindb.query(
        "SELECT id FROM admins WHERE username=? AND password=?", params);
    return !result.empty();
}

int admin(){
    initAdminTable();
    
    cout << "=== 管理员登录 ===" << endl;
    
    string username, password;
    int attempts = 0;
    
    while(attempts < 5){
        cout << "请输入管理员账号：";
        getline(cin, username);
        
        // 密码隐藏输入
        password = readPassword("请输入密码：", 19);

        if(verifyAdmin(username, password)){
            cout << "登录成功！" << endl;
            g_currentAdmin = username;  // 保存当前登录管理员
            return 1;
        }
        
        attempts++;
        cout << "账号或密码错误，还剩 " << (5 - attempts) << " 次机会" << endl;
    }
    
    cout << "登录失败次数过多，退出系统" << endl;
    return 0;
}
