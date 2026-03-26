#include <iostream>
#include <string>
#include "database.h"
#include "model.hpp"
#include "billingstandard.h"
#include "billingdatabase.h"

using namespace std;

void newstandard(void);// 新增计费标准
void searchstandard(void);// 查询计费标准
void changestandard(void);// 改动计费标准
void deletestandard(void);// 删除计费标准

void billingmenu(){
    char ch;
    while (true) {
        cout << "\n--------计费标准设置目录--------" << endl;
        cout << "1. 新增标准\n2. 查询标准\n3. 修改标准\n4. 删除标准\n0. 退出计费标准设置\n" << endl;
        cout << "输入数字指令以继续：";
        cin >> ch;
        cin.ignore(1024, '\n');
        switch (ch) {
            case '0':
                cout << "正在退出计费标准设置\n" << endl;
                return;
            case '1':
                cout << "新增计费标准\n" << endl;
                newstandard();
                break;
            case '2':
                cout << "查询计费标准\n" << endl;
                searchstandard();
                break;
            case '3':
                cout << "修改计费标准\n" << endl;
                changestandard();
                break;
            case '4':
                cout << "删除计费标准\n" << endl;
                deletestandard();
                break;
            default:
                cout << "输入指令不正确，请重新输入\n" << endl;
        }
    }
}

void newstandard(void){
    initbilling();

    Billing billing;
    cout << "请输入套餐编号：";
    cin >> billing.sPackageId;
    cin.ignore(1024, '\n');

    vector<const char*> params = {billing.sPackageId.c_str()};
    vector<vector<string>> result = billingdb.query("SELECT sPackageId FROM billings WHERE sPackageId=?", params);
    if(!result.empty()){
        cout << "套餐编号已存在！" << endl;
        return;
    }

    cout << "请选择计费单位（0-分钟，1-小时）：";
    int unitChoice;
    cin >> unitChoice;
    cin.ignore(1024, '\n');
    billing.nUnitType = static_cast<UnitType>(unitChoice);

    cout << "请输入单价（元）：";
    cin >> billing.fUnitPrice;
    cin.ignore(1024, '\n');

    billing.nDel = 0;

    string unitTypeStr = to_string(static_cast<int>(billing.nUnitType));
    string unitPriceStr = to_string(billing.fUnitPrice);
    string delStr = to_string(billing.nDel);
    vector<const char*> columns = {"sPackageId", "nUnitType", "fUnitPrice", "nDel"};
    vector<const char*> values = {
        billing.sPackageId.c_str(),
        unitTypeStr.c_str(),
        unitPriceStr.c_str(),
        delStr.c_str()
    };

    if(billingdb.insert("billings", columns, values)){
        cout << "添加套餐成功！" << endl;
    } else {
        cout << "添加失败：" << billingdb.getLastError() << endl;
    }
}// 新增计费标准

void searchstandard(void){
    initbilling();

    string id;
    cout << "请输入套餐编号（直接回车查询全部）：";
    getline(cin, id);

    vector<vector<string>> result;
    if(id.empty()){
        result = billingdb.query("SELECT sPackageId, nUnitType, fUnitPrice, nDel FROM billings WHERE nDel=0");
    } else {
        vector<const char*> params = {id.c_str()};
        result = billingdb.query("SELECT sPackageId, nUnitType, fUnitPrice, nDel FROM billings WHERE sPackageId=? AND nDel=0", params);
    }

    if(result.empty()){
        cout << "未找到套餐信息！" << endl;
        return;
    }

    cout << "套餐编号\t计费单位\t单价\t状态" << endl;
    for(size_t i = 0; i < result.size(); i++){
        Billing billing = queryToBilling(result, i);
        string unitTypeStr = (billing.nUnitType == UnitType::MINUTE) ? "分钟" : "小时";
        string statusStr = (billing.nDel == 0) ? "有效" : "失效";
        cout << billing.sPackageId << "\t\t" << unitTypeStr << "\t" << billing.fUnitPrice << "\t" << statusStr << endl;
    }
}// 查询计费标准

void changestandard(void){
    initbilling();

    string id;
    cout << "请输入要修改的套餐编号：";
    cin >> id;
    cin.ignore(1024, '\n');

    vector<const char*> params = {id.c_str()};
    vector<vector<string>> result = billingdb.query("SELECT sPackageId, nUnitType, fUnitPrice, nDel FROM billings WHERE sPackageId=? AND nDel=0", params);
    if(result.empty()){
        cout << "套餐不存在或已失效！" << endl;
        return;
    }

    Billing billing = queryToBilling(result, 0);
    cout << "当前套餐信息：" << endl;
    cout << "套餐编号：" << billing.sPackageId << endl;
    cout << "计费单位：" << (billing.nUnitType == UnitType::MINUTE ? "分钟" : "小时") << endl;
    cout << "单价：" << billing.fUnitPrice << "元" << endl;

    cout << "请选择需要修改的部分：\n1. 计费单位\n2. 单价" << endl;
    cout << "输入数字标号：";
    int cmd;
    cin >> cmd;
    cin.ignore(1024, '\n');

    if(cmd == 1){
        cout << "请选择新的计费单位（0-分钟，1-小时）：";
        int unitChoice;
        cin >> unitChoice;
        cin.ignore(1024, '\n');
        billing.nUnitType = static_cast<UnitType>(unitChoice);
    } else if(cmd == 2){
        cout << "请输入新的单价（元）：";
        cin >> billing.fUnitPrice;
        cin.ignore(1024, '\n');
    } else {
        cout << "输入指令错误。" << endl;
        return;
    }

    string unitTypeStr = to_string(static_cast<int>(billing.nUnitType));
    string unitPriceStr = to_string(billing.fUnitPrice);
    vector<const char*> updateParams = {unitTypeStr.c_str(), unitPriceStr.c_str(), id.c_str()};
    if(billingdb.update("billings", "nUnitType=?, fUnitPrice=?", "sPackageId=?", updateParams)){
        cout << "修改成功！" << endl;
    } else {
        cout << "修改失败：" << billingdb.getLastError() << endl;
    }
}// 改动计费标准

void deletestandard(void){
    initbilling();

    string id;
    cout << "请输入要删除的套餐编号：";
    cin >> id;
    cin.ignore(1024, '\n');

    if(id == "0"){
        cout << "默认套餐不能删除！" << endl;
        return;
    }

    vector<const char*> params = {id.c_str()};
    vector<vector<string>> result = billingdb.query("SELECT sPackageId FROM billings WHERE sPackageId=? AND nDel=0", params);
    if(result.empty()){
        cout << "套餐不存在或已失效！" << endl;
        return;
    }

    string delStr = to_string(1);
    vector<const char*> updateParams = {delStr.c_str(), id.c_str()};
    if(billingdb.update("billings", "nDel=?", "sPackageId=?", updateParams)){
        cout << "删除成功！" << endl;
    } else {
        cout << "删除失败：" << billingdb.getLastError() << endl;
    }
}// 删除计费标准