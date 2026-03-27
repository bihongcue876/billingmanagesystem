#include <iostream>
#include <string>
#include "database.h"
#include "model.hpp"
#include "financeservice.h"
#include "financedatabase.h"
#include "accountdatabase.h"

using namespace std;

void topup(void);// 充值
void refund(void);// 退费
void history(void);// 历史
void statistics(void);// 统计

void financemenu(){
    char ch;
    while (true) {
        cout << "\n--------财务服务目录--------" << endl;
        cout << "1. 账户充值\n2. 账户退费\n3. 账户消费记录\n4. 营业额统计\n0. 退出服务\n" << endl;
        cout << "输入数字指令以继续：";
        cin >> ch;
        cin.ignore(1024, '\n');
        switch (ch) {
            case '0':
                cout << "正在退出账户服务\n" << endl;
                return;
            case '1':
                cout << "账户充值\n" << endl;
                topup();
                break;
            case '2':
                cout << "账户退费\n" << endl;
                refund();
                break;
            case '3':
                cout << "账户消费记录\n" << endl;
                history();
                break;
            case '4':
                cout << "营业额统计\n" << endl;
                statistics();
                break;
            default:
                cout << "输入指令不正确，请重新输入\n" << endl;
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
    
    float newBalance = balanceBefore + amount;
    
    // 更新账户余额
    string balanceStr = to_string(newBalance);
    vector<const char*> updateParams = {balanceStr.c_str(), cardId};
    if(!accountdb.update("accounts", "fBalance=?", "aName=?", updateParams)){
        cout << "更新余额失败：" << accountdb.getLastError() << endl;
        return;
    }
    
    // 记录财务交易
    if(insertTransaction(cardId, 1, amount, balanceBefore, newBalance, "账户充值")){
        cout << "充值成功！当前余额：" << newBalance << "元" << endl;
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
        cout << "退费金额不能超过当前余额（" << balanceBefore << "元）！" << endl;
        return;
    }
    
    float newBalance = balanceBefore - amount;
    
    // 更新账户余额
    string balanceStr = to_string(newBalance);
    vector<const char*> updateParams = {balanceStr.c_str(), cardId};
    if(!accountdb.update("accounts", "fBalance=?", "aName=?", updateParams)){
        cout << "更新余额失败：" << accountdb.getLastError() << endl;
        return;
    }
    
    // 记录财务交易
    if(insertTransaction(cardId, 2, amount, balanceBefore, newBalance, "账户退费")){
        cout << "退费成功！当前余额：" << newBalance << "元" << endl;
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
    cout << "充值总额：" << totalTopup << " 元" << endl;
    cout << "退费总额：" << totalRefund << " 元" << endl;
    cout << "消费总额：" << totalConsumption << " 元" << endl;
    cout << "净营业额：" << (totalTopup - totalRefund) << " 元" << endl;
}// 统计