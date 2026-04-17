#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include "database.h"
#include "model.hpp"
#include "financeservice.h"
#include "financedatabase.h"
#include "accountdatabase.h"
#include "utils.hpp"

using namespace std;

void topup(void);// 充值
void refund(void);// 退费
void history(void);// 历史
void statistics(void);// 统计
void historyByDate(void);// 按日期查询

void financemenu(){
    char ch;
    while (true) {
        cout << endl;
        cout << "-----------------------------------" << endl;
        cout << "     财务服务目录" << endl;
        cout << "-----------------------------------" << endl;
        cout << "  1. 账户充值" << endl;
        cout << "  2. 账户退费" << endl;
        cout << "  3. 账户消费记录" << endl;
        cout << "  4. 营业额统计" << endl;
        cout << "  5. 按日期查询" << endl;
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
                cout << "\n>> 账户充值" << endl;
                topup();
                break;
            case '2':
                cout << "\n>> 账户退费" << endl;
                refund();
                break;
            case '3':
                cout << "\n>> 账户消费记录" << endl;
                history();
                break;
            case '4':
                cout << "\n>> 营业额统计" << endl;
                statistics();
                break;
            case '5':
                cout << "\n>> 按日期查询" << endl;
                historyByDate();
                break;
            default:
                cout << "\n输入指令不正确，请重新输入" << endl;
        }
    }
}

void topup(void){
    initfinance();
    
    char cardId[20];
    float amount;
    
    cout << "请输入卡号：";
    cin >> cardId;
    cin.ignore(1024, '\n');
    
    // 检查账户是否存在
    int status = searchaccount(cardId);
    if(status == 0){
        cout << "账户不存在！" << endl;
        return;
    }
    if(status == 2){
        cout << "账户已删除！" << endl;
        return;
    }
    
    // 获取当前余额（通过查询数据库）
    vector<const char*> params = {cardId};
    vector<vector<string>> result = accountdb.query(
        "SELECT fBalance FROM accounts WHERE aName=?", params);
    if(result.empty()){
        cout << "获取账户信息失败！" << endl;
        return;
    }
    float balanceBefore;
    try {
        balanceBefore = stof(result[0][0]);
    } catch (const std::exception& e) {
        cout << "账户余额数据格式错误！" << endl;
        return;
    }
    
    cout << "请输入充值金额：";
    cin >> amount;
    cin.ignore(1024, '\n');

    if(amount <= 0){
        cout << "充值金额必须大于 0！" << endl;
        return;
    }

    // 大额充值确认（超过 500 元）
    if(amount > 500){
        cout << "警告：充值金额较大（" << formatCurrency(amount) << " 元），确定继续吗？(y/n)：";
        char choice;
        cin >> choice;
        cin.ignore(1024, '\n');
        if(choice != 'y' && choice != 'Y'){
            cout << "已取消充值" << endl;
            return;
        }
    }

    float newBalance = round((balanceBefore + amount) * 100.0f) / 100.0f;
    
    // 更新账户余额
    string balanceStr = to_string(newBalance);
    vector<const char*> updateParams = {balanceStr.c_str(), cardId};
    if(!accountdb.update("accounts", "fBalance=?", "aName=?", updateParams)){
        cout << "更新余额失败：" << accountdb.getLastError() << endl;
        return;
    }
    
    // 记录财务交易
    if(insertTransaction(cardId, 1, amount, balanceBefore, newBalance, "账户充值")){
        cout << "充值成功！当前余额：" << formatCurrency(newBalance) << "元" << endl;
    } else {
        cout << "充值成功，但记录交易失败！" << endl;
    }
}// 充值

void refund(void){
    initfinance();
    
    char cardId[20];
    float amount;
    
    cout << "请输入卡号：";
    cin >> cardId;
    cin.ignore(1024, '\n');
    
    // 检查账户是否存在
    int status = searchaccount(cardId);
    if(status == 0){
        cout << "账户不存在！" << endl;
        return;
    }
    if(status == 2){
        cout << "账户已删除！" << endl;
        return;
    }
    
    // 获取当前余额
    vector<const char*> params = {cardId};
    vector<vector<string>> result = accountdb.query(
        "SELECT fBalance FROM accounts WHERE aName=?", params);
    if(result.empty()){
        cout << "获取账户信息失败！" << endl;
        return;
    }
    float balanceBefore;
    try {
        balanceBefore = stof(result[0][0]);
    } catch (const std::exception& e) {
        cout << "账户余额数据格式错误！" << endl;
        return;
    }
    
    cout << "请输入退费金额：";
    cin >> amount;
    cin.ignore(1024, '\n');

    if(amount <= 0){
        cout << "退费金额必须大于 0！" << endl;
        return;
    }

    if(amount > balanceBefore){
        cout << "退费金额不能超过当前余额（" << formatCurrency(balanceBefore) << "元）！" << endl;
        return;
    }

    // 大额退费确认（超过 500 元）
    if(amount > 500){
        cout << "警告：退费金额较大（" << formatCurrency(amount) << " 元），确定继续吗？(y/n)：";
        char choice;
        cin >> choice;
        cin.ignore(1024, '\n');
        if(choice != 'y' && choice != 'Y'){
            cout << "已取消退费" << endl;
            return;
        }
    }

    float newBalance = round((balanceBefore - amount) * 100.0f) / 100.0f;
    
    // 更新账户余额
    string balanceStr = to_string(newBalance);
    vector<const char*> updateParams = {balanceStr.c_str(), cardId};
    if(!accountdb.update("accounts", "fBalance=?", "aName=?", updateParams)){
        cout << "更新余额失败：" << accountdb.getLastError() << endl;
        return;
    }
    
    // 记录财务交易
    if(insertTransaction(cardId, 2, amount, balanceBefore, newBalance, "账户退费")){
        cout << "退费成功！当前余额：" << formatCurrency(newBalance) << "元" << endl;
    } else {
        cout << "退费成功，但记录交易失败！" << endl;
    }
}// 退费

void history(void){
    initfinance();
    
    char cardId[20];
    cout << "请输入卡号（直接回车查询全部）：";
    cin.getline(cardId, 20);
    
    vector<Finance> records;
    if(string(cardId).empty()){
        records = queryAllTransactions();
    } else {
        records = queryHistory(cardId);
    }
    
    if(records.empty()){
        cout << "未找到交易记录！" << endl;
        return;
    }
    
    cout << "ID\t卡号\t类型\t金额\t备注\t时间" << endl;
    for(size_t i = 0; i < records.size(); i++){
        string typeStr;
        if(records[i].type == 1) typeStr = "充值";
        else if(records[i].type == 2) typeStr = "退费";
        else if(records[i].type == 3) typeStr = "消费";
        else typeStr = "未知";
        
        cout << records[i].id << "\t" << records[i].cardId << "\t" 
             << typeStr << "\t" << records[i].amount << "\t" 
             << records[i].remark << "\t" << records[i].time << endl;
    }
}// 历史

void statistics(void){
    initfinance();
    
    float totalTopup = sumAmountByType(1);    // 充值总额
    float totalRefund = sumAmountByType(2);   // 退费总额
    float totalConsumption = sumAmountByType(3); // 消费总额
    
    cout << "--------营业额统计--------" << endl;
    cout << "充值总额：" << formatCurrency(totalTopup) << " 元" << endl;
    cout << "退费总额：" << formatCurrency(totalRefund) << " 元" << endl;
    cout << "消费总额：" << formatCurrency(totalConsumption) << " 元" << endl;
    cout << "净营业额：" << formatCurrency(totalTopup - totalRefund) << " 元" << endl;
}// 统计

void historyByDate(void){
    initfinance();

    string startDate, endDate;
    cout << "请输入开始日期（格式：YYYY-MM-DD）：";
    getline(cin, startDate);
    cout << "请输入结束日期（格式：YYYY-MM-DD）：";
    getline(cin, endDate);

    if(startDate.empty() || endDate.empty()){
        cout << "日期不能为空！" << endl;
        return;
    }

    // 将日期扩展为完整的日期时间范围
    // 开始日期：YYYY-MM-DD 00:00:00
    // 结束日期：YYYY-MM-DD 23:59:59
    string startDateTime = startDate + " 00:00:00";
    string endDateTime = endDate + " 23:59:59";

    // 查询指定日期范围的交易记录
    vector<const char*> params = {startDateTime.c_str(), endDateTime.c_str()};
    vector<vector<string>> result = financedb.query(
        "SELECT id, card_id, type, amount, remark, time FROM finance "
        "WHERE time >= ? AND time <= ? ORDER BY time DESC", params);

    if(result.empty()){
        cout << "未找到指定日期范围的交易记录！" << endl;
        return;
    }

    cout << "ID\t卡号\t类型\t金额\t备注\t时间" << endl;
    cout << "------------------------------------------------------------" << endl;

    float totalAmount = 0.0f;
    int count = 0;

    for(size_t i = 0; i < result.size(); i++){
        const vector<string>& row = result[i];
        if(row.size() < 6) continue;

        string typeStr;
        int type = stoi(row[2]);
        float amount = stof(row[3]);

        if(type == 1) typeStr = "充值";
        else if(type == 2) typeStr = "退费";
        else if(type == 3) typeStr = "消费";
        else typeStr = "未知";

        cout << row[0] << "\t" << row[1] << "\t"
             << typeStr << "\t" << amount << "\t"
             << row[4] << "\t" << row[5] << endl;

        if(type == 1) totalAmount += amount;
        else if(type == 2) totalAmount -= amount;
        count++;
    }

    cout << "------------------------------------------------------------" << endl;
    cout << "共计 " << count << " 条记录，净营业额：" << totalAmount << " 元" << endl;
}// 按日期查询