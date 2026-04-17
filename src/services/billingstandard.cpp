#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include "database.h"
#include "model.hpp"
#include "billingstandard.h"
#include "billingdatabase.h"
#include "utils.hpp"

using namespace std;

void newstandard(void);// 新增计费标准
void searchstandard(void);// 查询计费标准
void changestandard(void);// 改动计费标准
void deletestandard(void);// 删除计费标准

void billingmenu(){
    char ch;
    while (true) {
        cout << endl;
        cout << "-----------------------------------" << endl;
        cout << "     计费标准设置目录" << endl;
        cout << "-----------------------------------" << endl;
        cout << "  1. 新增标准" << endl;
        cout << "  2. 查询标准" << endl;
        cout << "  3. 修改标准" << endl;
        cout << "  4. 删除标准" << endl;
        cout << "  0. 返回主菜单" << endl;
        cout << "-----------------------------------" << endl;
        cout << "请输入选项：";
        cin >> ch;
        cin.ignore(1024, '\n');
        switch (ch) {
            case '0':
                cout << "\n正在返回主菜单..." << endl;
                return;
            case '1':
                cout << "\n>> 新增标准" << endl;
                newstandard();
                break;
            case '2':
                cout << "\n>> 查询标准" << endl;
                searchstandard();
                break;
            case '3':
                cout << "\n>> 修改标准" << endl;
                changestandard();
                break;
            case '4':
                cout << "\n>> 删除标准" << endl;
                deletestandard();
                break;
            default:
                cout << "\n输入指令不正确，请重新输入" << endl;
        }
    }
}

void newstandard(void){
    initbilling();

    Billing billing;
    cout << "请输入套餐编号：";
    cin >> billing.sPackageId;
    cin.ignore(1024, '\n');

    if(billing.sPackageId.empty()){
        cout << "套餐编号不能为空！" << endl;
        return;
    }

    vector<const char*> params = {billing.sPackageId.c_str()};
    vector<vector<string>> result = billingdb.query("SELECT sPackageId FROM billings WHERE sPackageId=? AND nDel=0", params);
    if(!result.empty()){
        cout << "套餐编号已存在！" << endl;
        return;
    }

    cout << "请输入套餐名称：";
    getline(cin, billing.sPackageName);

    // 计费单位选择，带输入验证
    while(true){
        cout << "请选择计费单位（0-分钟，1-小时）：";
        int unitChoice;
        cin >> unitChoice;
        if(cin.fail()){
            cin.clear();
            cin.ignore(1024, '\n');
            cout << "输入格式错误，请重新输入！" << endl;
            continue;
        }
        cin.ignore(1024, '\n');
        if(unitChoice != 0 && unitChoice != 1){
            cout << "无效选项，请输入 0 或 1！" << endl;
            continue;
        }
        billing.nUnitType = static_cast<UnitType>(unitChoice);
        break;
    }

    // 选择计费模式
    while(true){
        cout << "请选择计费模式（1-简单计费，2-分段计费）：";
        int modeChoice;
        cin >> modeChoice;
        if(cin.fail()){
            cin.clear();
            cin.ignore(1024, '\n');
            cout << "输入格式错误，请重新输入！" << endl;
            continue;
        }
        cin.ignore(1024, '\n');
        if(modeChoice != 1 && modeChoice != 2){
            cout << "无效选项，请输入 1 或 2！" << endl;
            continue;
        }
        
        if(modeChoice == 1) {
            billing.nSegmentCount = 0;
            while(true){
                cout << "请输入单价（元）：";
                float price;
                cin >> price;
                if(cin.fail()){
                    cin.clear();
                    cin.ignore(1024, '\n');
                    cout << "输入格式错误，请重新输入！" << endl;
                    continue;
                }
                cin.ignore(1024, '\n');
                if(price < 0){
                    cout << "单价不能为负数！" << endl;
                    continue;
                }
                billing.fUnitPrice = price;
                break;
            }
        } else {
            billing.fUnitPrice = 0;
            while(true){
                cout << "请输入分段数量（1-" << MAX_SEGMENTS << "）：";
                int segCount;
                cin >> segCount;
                if(cin.fail()){
                    cin.clear();
                    cin.ignore(1024, '\n');
                    cout << "输入格式错误，请重新输入！" << endl;
                    continue;
                }
                cin.ignore(1024, '\n');
                if(segCount < 1 || segCount > MAX_SEGMENTS){
                    cout << "分段数量无效！" << endl;
                    continue;
                }
                billing.nSegmentCount = segCount;
                break;
            }
            
            string segmentsStr = "";
            for(int i = 0; i < billing.nSegmentCount; i++){
                BillingSegment seg;
                cout << "\n--- 第 " << (i+1) << " 段 ---" << endl;
                
                while(true){
                    cout << "开始时间（小时，如0表示0小时起）：";
                    cin >> seg.fStartHour;
                    if(cin.fail()){
                        cin.clear();
                        cin.ignore(1024, '\n');
                        cout << "输入格式错误！" << endl;
                        continue;
                    }
                    cin.ignore(1024, '\n');
                    if(seg.fStartHour < 0){
                        cout << "开始时间不能为负数！" << endl;
                        continue;
                    }
                    break;
                }
                
                while(true){
                    cout << "结束时间（小时，-1表示无上限）：";
                    cin >> seg.fEndHour;
                    if(cin.fail()){
                        cin.clear();
                        cin.ignore(1024, '\n');
                        cout << "输入格式错误！" << endl;
                        continue;
                    }
                    cin.ignore(1024, '\n');
                    if(seg.fEndHour != -1 && seg.fEndHour <= seg.fStartHour){
                        cout << "结束时间必须大于开始时间！" << endl;
                        continue;
                    }
                    break;
                }
                
                while(true){
                    cout << "该段单价（元/小时）：";
                    cin >> seg.fPricePerHour;
                    if(cin.fail()){
                        cin.clear();
                        cin.ignore(1024, '\n');
                        cout << "输入格式错误！" << endl;
                        continue;
                    }
                    cin.ignore(1024, '\n');
                    if(seg.fPricePerHour < 0){
                        cout << "单价不能为负数！" << endl;
                        continue;
                    }
                    break;
                }
                
                billing.segments[i] = seg;
                
                if(i > 0){
                    segmentsStr += ";";
                }
                segmentsStr += to_string(seg.fStartHour) + "," + to_string(seg.fEndHour) + "," + to_string(seg.fPricePerHour);
            }
            
            if(segmentsStr.empty()){
                segmentsStr = "";
            }
            
            string segCountStr = to_string(billing.nSegmentCount);
            vector<const char*> segColumns = {"sPackageId", "sPackageName", "nUnitType", "fUnitPrice", "nSegmentCount", "sSegments", "nDel"};
            vector<const char*> segValues = {
                billing.sPackageId.c_str(),
                billing.sPackageName.c_str(),
                to_string(static_cast<int>(billing.nUnitType)).c_str(),
                to_string(billing.fUnitPrice).c_str(),
                segCountStr.c_str(),
                segmentsStr.c_str(),
                "0"
            };
            
            if(billingdb.insert("billings", segColumns, segValues)){
                cout << "添加套餐成功！" << endl;
            } else {
                cout << "添加失败：" << billingdb.getLastError() << endl;
            }
            return;
        }
        break;
    }

    billing.nDel = 0;

    string unitTypeStr = to_string(static_cast<int>(billing.nUnitType));
    string unitPriceStr = to_string(billing.fUnitPrice);
    string segmentCountStr = to_string(billing.nSegmentCount);
    string delStr = to_string(billing.nDel);
    vector<const char*> columns = {"sPackageId", "sPackageName", "nUnitType", "fUnitPrice", "nSegmentCount", "sSegments", "nDel"};
    vector<const char*> values = {
        billing.sPackageId.c_str(),
        billing.sPackageName.c_str(),
        unitTypeStr.c_str(),
        unitPriceStr.c_str(),
        segmentCountStr.c_str(),
        "",
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
        result = billingdb.query("SELECT sPackageId, sPackageName, nUnitType, fUnitPrice, nSegmentCount, sSegments, nDel FROM billings WHERE nDel=0");
    } else {
        vector<const char*> params = {id.c_str()};
        result = billingdb.query("SELECT sPackageId, sPackageName, nUnitType, fUnitPrice, nSegmentCount, sSegments, nDel FROM billings WHERE sPackageId=? AND nDel=0", params);
    }

    if(result.empty()){
        cout << "未找到套餐信息！" << endl;
        return;
    }

    cout << "套餐编号\t套餐名称\t\t计费模式\t计费单位\t单价\t状态" << endl;
    string outputContent = "套餐编号,套餐名称,计费模式,计费单位,单价,状态\n";
    for(size_t i = 0; i < result.size(); i++){
        Billing billing = queryToBilling(result, i);
        string unitTypeStr = (billing.nUnitType == UnitType::MINUTE) ? "分钟" : "小时";
        string modeStr = (billing.nSegmentCount > 0) ? "分段计费" : "简单计费";
        string statusStr = (billing.nDel == 0) ? "有效" : "失效";
        string line = billing.sPackageId + "\t" + billing.sPackageName + "\t\t" + modeStr + "\t\t" + unitTypeStr + "\t" + formatCurrency(billing.fUnitPrice) + "\t" + statusStr + "\n";
        cout << line;
        outputContent += billing.sPackageId + "," + billing.sPackageName + "," + modeStr + "," + unitTypeStr + "," + formatCurrency(billing.fUnitPrice) + "," + statusStr + "\n";
        
        if(billing.nSegmentCount > 0){
            cout << "  分段详情：" << endl;
            for(int j = 0; j < billing.nSegmentCount; j++){
                BillingSegment& seg = billing.segments[j];
                string endStr = (seg.fEndHour == -1) ? "无上限" : to_string(seg.fEndHour) + "小时";
                cout << "    第" << (j+1) << "段: " << seg.fStartHour << "小时 ~ " << endStr << ", 单价: " << formatCurrency(seg.fPricePerHour) << "元/小时" << endl;
            }
        }
    }

    cout << "\n是否导出到文件？(y/n)：";
    char choice;
    cin >> choice;
    cin.ignore(1024, '\n');

    if(choice == 'y' || choice == 'Y'){
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        char filename[64];
        strftime(filename, sizeof(filename), "billing_standards_%Y%m%d_%H%M%S.csv", timeinfo);

        string outputDir = OUTPUT_ROOT;
        string filepath = outputDir + filename;
        if(ensureDirectory(outputDir)){
            if(saveToFile(filepath, outputContent)){
                cout << "导出成功！文件保存至：" << filepath << endl;
            } else {
                cout << "导出失败！" << endl;
            }
        } else {
            cout << "创建输出目录失败！" << endl;
        }
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
    cout << "单价：" << formatCurrency(billing.fUnitPrice) << "元" << endl;

    cout << "请选择需要修改的部分：\n1. 计费单位\n2. 单价" << endl;
    cout << "输入数字标号：";
    int cmd;
    cin >> cmd;
    if(cin.fail()){
        cin.clear();
        cin.ignore(1024, '\n');
        cout << "输入指令错误。" << endl;
        return;
    }
    cin.ignore(1024, '\n');

    if(cmd == 1){
        // 计费单位选择，带输入验证
        while(true){
            cout << "请选择新的计费单位（0-分钟，1-小时）：";
            int unitChoice;
            cin >> unitChoice;
            if(cin.fail()){
                cin.clear();
                cin.ignore(1024, '\n');
                cout << "输入格式错误，请重新输入！" << endl;
                continue;
            }
            cin.ignore(1024, '\n');
            if(unitChoice != 0 && unitChoice != 1){
                cout << "无效选项，请输入 0 或 1！" << endl;
                continue;
            }
            billing.nUnitType = static_cast<UnitType>(unitChoice);
            break;
        }
    } else if(cmd == 2){
        // 单价输入，带输入验证
        while(true){
            cout << "请输入新的单价（元）：";
            float price;
            cin >> price;
            if(cin.fail()){
                cin.clear();
                cin.ignore(1024, '\n');
                cout << "输入格式错误，请重新输入！" << endl;
                continue;
            }
            cin.ignore(1024, '\n');
            if(price < 0){
                cout << "单价不能为负数！" << endl;
                continue;
            }
            billing.fUnitPrice = price;
            break;
        }
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