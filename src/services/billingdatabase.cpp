// 计费原则数据库
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

#include "database.h"
#include "model.hpp"
#include "billingstandard.h"
#include "billingdatabase.h"

using namespace std;

sqlitedb billingdb(DATA_ROOT"billing.db");

void initbilling(){
    const char* columnDefs = "sPackageId TEXT PRIMARY KEY, sPackageName TEXT, nUnitType INTEGER, fUnitPrice REAL, nSegmentCount INTEGER, sSegments TEXT, nDel INTEGER";
    billingdb.tablecreate("billings", columnDefs);
    
    // 检查默认套餐"0"是否存在，不存在则创建
    vector<vector<string>> checkResult = billingdb.query("SELECT sPackageId FROM billings WHERE sPackageId='0'");
    if(checkResult.empty()){
        vector<const char*> columns = {"sPackageId", "sPackageName", "nUnitType", "fUnitPrice", "nSegmentCount", "sSegments", "nDel"};
        string unitTypeStr = to_string(static_cast<int>(UnitType::MINUTE));
        string unitPriceStr = to_string(0.1f);
        string segmentCountStr = to_string(0);
        string segmentsStr = "";
        string delStr = to_string(0);
        vector<const char*> values = {"0", "默认套餐", unitTypeStr.c_str(), unitPriceStr.c_str(), segmentCountStr.c_str(), segmentsStr.c_str(), delStr.c_str()};
        billingdb.insert("billings", columns, values);
    }
}

bool newstnd(void){
    initbilling();
    
    vector<const char*> checkParams = {"0"};
    vector<vector<string>> checkResult = billingdb.query("SELECT sPackageId FROM billings WHERE sPackageId=?", checkParams);
    if(checkResult.empty()){
        vector<const char*> columns = {"sPackageId", "nUnitType", "fUnitPrice", "nDel"};
        string unitTypeStr = to_string(static_cast<int>(UnitType::MINUTE));
        string unitPriceStr = to_string(0.1f);
        string delStr = to_string(0);
        vector<const char*> values = {"0", unitTypeStr.c_str(), unitPriceStr.c_str(), delStr.c_str()};
        if(!billingdb.insert("billings", columns, values)){
            cout << "默认套餐初始化失败：" << billingdb.getLastError() << endl;
            return false;
        }
    } // 默认情况：套餐0
    
    Billing billing;
    cout << "请输入套餐编号：";
    cin >> billing.sPackageId;
    cin.ignore(1024, '\n');
    
    vector<const char*> params = {billing.sPackageId.c_str()};
    vector<vector<string>> result = billingdb.query("SELECT sPackageId FROM billings WHERE sPackageId=?", params);
    if(!result.empty()){
        cout << "套餐编号已存在，不可重复添加！" << endl;
        return false;
    }
    
    int unitChoice;
    while(true){
        cout << "请选择计费单位（0-分钟，1-小时）：";
        cin >> unitChoice;
        if(cin.fail()){
            cin.clear();
            cin.ignore(1024, '\n');
            cout << "输入格式错误，请重新输入！" << endl;
            continue;
        }
        cin.ignore(1024, '\n');
        if(unitChoice == 0 || unitChoice == 1){
            billing.nUnitType = static_cast<UnitType>(unitChoice);
            break;
        } else {
            cout << "计费单位无效，请输入0或1！" << endl;
        }
    }
    
    while(true){
        cout << "请输入单价（元）：";
        cin >> billing.fUnitPrice;
        if(cin.fail()){
            cin.clear();
            cin.ignore(1024, '\n');
            cout << "输入格式错误，请重新输入！" << endl;
            continue;
        }
        cin.ignore(1024, '\n');
        if(billing.fUnitPrice <= 0){
            cout << "单价必须大于0，请重新输入！" << endl;
        } else {
            break;
        }
    }
    
    billing.nDel = 0;
    
    vector<const char*> columns = {"sPackageId", "nUnitType", "fUnitPrice", "nDel"};
    string unitTypeStr = to_string(static_cast<int>(billing.nUnitType));
    string unitPriceStr = to_string(billing.fUnitPrice);
    string delStr = to_string(billing.nDel);
    vector<const char*> values = {
        billing.sPackageId.c_str(),
        unitTypeStr.c_str(),
        unitPriceStr.c_str(),
        delStr.c_str()
    };
    
    if(billingdb.insert("billings", columns, values)){
        cout << "添加套餐成功！" << endl;
        return true;
    } else {
        cout << "添加套餐失败：" << billingdb.getLastError() << endl;
        return false;
    }
} // 新建套餐标准

bool searchstnd(const string& id){
    initbilling();
    
    vector<vector<string>> result;
    if(id.empty()){
        result = billingdb.query("SELECT sPackageId, sPackageName, nUnitType, fUnitPrice, nSegmentCount, sSegments, nDel FROM billings WHERE nDel=0");
    } else {
        vector<const char*> params = {id.c_str()};
        result = billingdb.query("SELECT sPackageId, sPackageName, nUnitType, fUnitPrice, nSegmentCount, sSegments, nDel FROM billings WHERE sPackageId=? AND nDel=0", params);
    }
    
    if(result.empty()){
        cout << "未找到套餐信息！" << endl;
        return false;
    }
    
    cout << "套餐编号\t计费单位\t单价\t状态" << endl;
    for(size_t i = 0; i < result.size(); i++){
        Billing billing = queryToBilling(result, i);
        string unitTypeStr = (billing.nUnitType == UnitType::MINUTE) ? "分钟" : "小时";
        string statusStr = (billing.nDel == 0) ? "有效" : "失效";
        cout << billing.sPackageId << "\t\t" << unitTypeStr << "\t" << billing.fUnitPrice << "\t" << statusStr << endl;
    }
    return true;
} // 查询套餐标准

bool changestnd(const std::string& id){
    initbilling();
    
    vector<const char*> params = {id.c_str()};
    vector<vector<string>> result = billingdb.query("SELECT sPackageId, sPackageName, nUnitType, fUnitPrice, nSegmentCount, sSegments, nDel FROM billings WHERE sPackageId=? AND nDel=0", params);
    if(result.empty()){
        cout << "套餐不存在或已失效！" << endl;
        return false;
    }
    
    Billing billing = queryToBilling(result, 0);
    cout << "当前套餐信息：" << endl;
    cout << "套餐编号：" << billing.sPackageId << endl;
    cout << "计费单位：" << (billing.nUnitType == UnitType::MINUTE ? "分钟" : "小时") << endl;
    cout << "单价：" << billing.fUnitPrice << "元" << endl;
    
    cout << "请选择需要修改的部分：\n1.计费单位\n2.单价" << endl;
    cout << "输入数字标号：";
    int cmd;
    cin >> cmd;
    cin.ignore(1024, '\n');
    
    switch(cmd){
        case 1:
            {
            int unitChoice;
            while(true){
                cout << "请选择新的计费单位（0-分钟，1-小时）：";
                cin >> unitChoice;
                if(cin.fail()){
                    cin.clear();
                    cin.ignore(1024, '\n');
                    cout << "输入格式错误，请重新输入！" << endl;
                    continue;
                }
                cin.ignore(1024, '\n');
                if(unitChoice == 0 || unitChoice == 1){
                    billing.nUnitType = static_cast<UnitType>(unitChoice);
                    break;
                } else {
                    cout << "计费单位无效，请输入0或1！" << endl;
                }
            }
            break;
            }
        case 2:
            while(true){
                cout << "请输入新的单价（元）：";
                cin >> billing.fUnitPrice;
                if(cin.fail()){
                    cin.clear();
                    cin.ignore(1024, '\n');
                    cout << "输入格式错误，请重新输入！" << endl;
                    continue;
                }
                cin.ignore(1024, '\n');
                if(billing.fUnitPrice <= 0){
                    cout << "单价必须大于0，请重新输入！" << endl;
                } else {
                    break;
                }
            }
            break;
        default:
            cout << "输入指令错误。" << endl;
            return false;
    }
    
    string unitTypeStr = to_string(static_cast<int>(billing.nUnitType));
    string unitPriceStr = to_string(billing.fUnitPrice);
    vector<const char*> updateParams = {unitTypeStr.c_str(), unitPriceStr.c_str(), id.c_str()};
    if(billingdb.update("billings", "nUnitType=?, fUnitPrice=?", "sPackageId=?", updateParams)){
        cout << "修改成功！" << endl;
        return true;
    } else {
        cout << "修改失败：" << billingdb.getLastError() << endl;
        return false;
    }
} // 修改套餐标准

bool deletestnd(const string& id){
    initbilling();
    
    if(id == "0"){
        cout << "默认套餐不能删除！" << endl;
        return false;
    }
    
    vector<const char*> params = {id.c_str()};
    vector<vector<string>> result = billingdb.query("SELECT sPackageId FROM billings WHERE sPackageId=? AND nDel=0", params);
    if(result.empty()){
        cout << "套餐不存在或已失效！" << endl;
        return false;
    }
    
    int nDel = 1;
    string delStr = to_string(nDel);
    vector<const char*> updateParams = {delStr.c_str(), id.c_str()};
    if(billingdb.update("billings", "nDel=?", "sPackageId=?", updateParams)){
        cout << "删除成功！" << endl;
        return true;
    } else {
        cout << "删除失败：" << billingdb.getLastError() << endl;
        return false;
    }
} // 删除套餐标准

Billing queryToBilling(const vector<vector<string>>& result, int index){
    Billing billing;
    billing.sPackageId = "";
    billing.sPackageName = "";
    billing.nUnitType = UnitType::MINUTE;
    billing.fUnitPrice = 0.0f;
    billing.nSegmentCount = 0;
    for(int i = 0; i < MAX_SEGMENTS; i++) {
        billing.segments[i].fStartHour = 0;
        billing.segments[i].fEndHour = 0;
        billing.segments[i].fPricePerHour = 0;
    }
    billing.nDel = 0;
    
    if(index >= 0 && index < result.size()){
        const auto& row = result[index];
        if(row.size() >= 7){
            try {
                billing.sPackageId = row[0];
                billing.sPackageName = row[1];
                int unitTypeValue = stoi(row[2]);
                billing.nUnitType = static_cast<UnitType>(unitTypeValue);
                billing.fUnitPrice = stof(row[3]);
                billing.nSegmentCount = stoi(row[4]);
                
                // 解析分段数据 JSON格式: start1,end1,price1;start2,end2,price2;...
                if(!row[5].empty() && billing.nSegmentCount > 0) {
                    string segmentsStr = row[5];
                    int segIndex = 0;
                    size_t pos = 0;
                    while(segIndex < MAX_SEGMENTS && pos < segmentsStr.size()) {
                        size_t comma1 = segmentsStr.find(',', pos);
                        if(comma1 == string::npos) break;
                        size_t comma2 = segmentsStr.find(',', comma1 + 1);
                        if(comma2 == string::npos) break;
                        size_t semicolon = segmentsStr.find(';', comma2);
                        if(semicolon == string::npos) semicolon = segmentsStr.size();
                        
                        billing.segments[segIndex].fStartHour = stof(segmentsStr.substr(pos, comma1 - pos));
                        billing.segments[segIndex].fEndHour = stof(segmentsStr.substr(comma1 + 1, comma2 - comma1 - 1));
                        billing.segments[segIndex].fPricePerHour = stof(segmentsStr.substr(comma2 + 1, semicolon - comma2 - 1));
                        
                        pos = semicolon + 1;
                        segIndex++;
                    }
                }
                
                billing.nDel = stoi(row[6]);
            } catch (const std::exception& e) {
                // 数据格式错误，返回零初始化的对象
            }
        }
    }
    return billing;
}