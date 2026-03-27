#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include "adminservice.h"
#include "database.h"

using namespace std;

extern string g_currentAdmin;

// 密码隐藏输入
static string readPassword(const string& prompt){
    cout << prompt;
    int pwdIndex = 0;
    char ch;
    char inputPwd[20] = {0};
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (pwdIndex > 0) {
                pwdIndex--;
                cout << "\b \b";
            }
        } else if (pwdIndex < 19) {
            inputPwd[pwdIndex] = ch;
            cout << '*';
            pwdIndex++;
        }
    }
    inputPwd[pwdIndex] = '\0';
    cout << endl;
    return string(inputPwd);
}

// 验证当前管理员密码
static bool verifyCurrentPassword(const string& username, const string& password){
    sqlitedb admindb(DATA_ROOT"admin.db");
    vector<const char*> params = {username.c_str(), password.c_str()};
    vector<vector<string>> result = admindb.query(
        "SELECT id FROM admins WHERE username=? AND password=?", params);
    return !result.empty();
}

// 修改密码
void changePassword(const string& currentAdmin){
    cout << "=== 修改密码 ===" << endl;

    string oldPwd = readPassword("请输入当前密码：");
    
    if(!verifyCurrentPassword(currentAdmin, oldPwd)){
        cout << "当前密码错误！" << endl;
        return;
    }

    string newPwd = readPassword("请输入新密码（6-12 位）：");
    
    if(newPwd.length() < 6 || newPwd.length() > 12){
        cout << "密码长度必须在 6-12 位之间！" << endl;
        return;
    }

    string confirmPwd = readPassword("请确认新密码：");
    
    if(newPwd != confirmPwd){
        cout << "两次输入的密码不一致！" << endl;
        return;
    }

    sqlitedb admindb(DATA_ROOT"admin.db");
    vector<const char*> params = {newPwd.c_str(), currentAdmin.c_str()};
    if(admindb.update("admins", "password=?", "username=?", params)){
        cout << "密码修改成功！" << endl;
    } else {
        cout << "密码修改失败：" << admindb.getLastError() << endl;
    }
}

// 添加管理员
void addAdmin(const string& currentAdmin){
    cout << "=== 添加管理员 ===" << endl;

    string username;
    cout << "请输入新管理员账号（6-12 位）：";
    getline(cin, username);

    if(username.length() < 6 || username.length() > 12){
        cout << "账号长度必须在 6-12 位之间！" << endl;
        return;
    }

    sqlitedb admindb(DATA_ROOT"admin.db");
    vector<const char*> checkParams = {username.c_str()};
    vector<vector<string>> checkResult = admindb.query(
        "SELECT id FROM admins WHERE username=?", checkParams);
    if(!checkResult.empty()){
        cout << "该账号已存在！" << endl;
        return;
    }

    string password = readPassword("请输入密码（6-12 位）：");
    
    if(password.length() < 6 || password.length() > 12){
        cout << "密码长度必须在 6-12 位之间！" << endl;
        return;
    }

    string confirmPwd = readPassword("请确认密码：");
    
    if(password != confirmPwd){
        cout << "两次输入的密码不一致！" << endl;
        return;
    }

    vector<const char*> columns = {"username", "password"};
    vector<const char*> values = {username.c_str(), password.c_str()};
    if(admindb.insert("admins", columns, values)){
        cout << "管理员添加成功！" << endl;
    } else {
        cout << "管理员添加失败：" << admindb.getLastError() << endl;
    }
}

// 删除管理员
void deleteAdmin(){
    cout << "=== 删除管理员 ===" << endl;

    string username;
    cout << "请输入要删除的管理员账号：";
    getline(cin, username);

    if(username == g_currentAdmin){
        cout << "不能删除自己！" << endl;
        return;
    }

    sqlitedb admindb(DATA_ROOT"admin.db");
    vector<const char*> params = {username.c_str()};
    vector<vector<string>> result = admindb.query(
        "SELECT id FROM admins WHERE username=?", params);
    if(result.empty()){
        cout << "该账号不存在！" << endl;
        return;
    }

    cout << "确定要删除管理员 [" << username << "] 吗？(y/n)：";
    char choice;
    cin >> choice;
    cin.ignore(1024, '\n');

    if(choice != 'y' && choice != 'Y'){
        cout << "已取消删除" << endl;
        return;
    }

    if(admindb.remove("admins", "username=?", params)){
        cout << "管理员删除成功！" << endl;
    } else {
        cout << "管理员删除失败：" << admindb.getLastError() << endl;
    }
}

// 管理员列表
void adminList(){
    cout << "=== 管理员列表 ===" << endl;

    sqlitedb admindb(DATA_ROOT"admin.db");
    vector<vector<string>> result = admindb.query("SELECT id, username, createTime FROM admins ORDER BY id");

    if(result.empty()){
        cout << "暂无管理员" << endl;
        return;
    }

    cout << "ID\t账号\t创建时间" << endl;
    cout << "------------------------" << endl;
    for(size_t i = 0; i < result.size(); i++){
        cout << result[i][0] << "\t" << result[i][1] << "\t" << result[i][2] << endl;
    }
    cout << "------------------------" << endl;
    cout << "共计 " << result.size() << " 个管理员账号" << endl;
}

// 管理员管理菜单
void adminmenu(){
    char ch;
    while (true) {
        cout << "\n--------管理员管理--------" << endl;
        cout << "1. 修改密码\n2. 添加管理员\n3. 删除管理员\n4. 管理员列表\n0. 返回主菜单\n" << endl;
        cout << "输入数字指令以继续：";
        cin >> ch;
        cin.ignore(1024, '\n');

        switch (ch) {
            case '0':
                cout << "正在返回主菜单\n" << endl;
                return;

            case '1':
                cout << "修改密码\n" << endl;
                changePassword(g_currentAdmin);
                break;

            case '2':
                cout << "添加管理员\n" << endl;
                addAdmin(g_currentAdmin);
                break;

            case '3':
                cout << "删除管理员\n" << endl;
                deleteAdmin();
                break;

            case '4':
                cout << "管理员列表\n" << endl;
                adminList();
                break;

            default:
                cout << "输入指令不正确，请重新输入\n" << endl;
                break;
        }
    }
}
