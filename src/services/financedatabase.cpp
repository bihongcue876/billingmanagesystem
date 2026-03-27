#include <iostream>
#include <vector>
#include <string>
#include "database.h"
#include "model.hpp"
#include "financedatabase.h"

using namespace std;

sqlitedb financedb(DATA_ROOT"finance.db");

void initfinance() {
    const char* columnDefs = 
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "card_id TEXT NOT NULL, "
        "type INTEGER NOT NULL, "
        "amount REAL NOT NULL, "
        "balance_before REAL NOT NULL, "
        "balance_after REAL NOT NULL, "
        "remark TEXT, "
        "time DATETIME DEFAULT CURRENT_TIMESTAMP";
    financedb.tablecreate("finance", columnDefs);
}

bool insertTransaction(string cardId, int type, float amount,
                       float balanceBefore, float balanceAfter,
                       string remark) {
    initfinance();
    
    string typeStr = to_string(type);
    string amountStr = to_string(amount);
    string balanceBeforeStr = to_string(balanceBefore);
    string balanceAfterStr = to_string(balanceAfter);
    
    vector<const char*> columns = {"card_id", "type", "amount", 
                                    "balance_before", "balance_after", "remark"};
    vector<const char*> values = {cardId.c_str(), typeStr.c_str(), 
                                   amountStr.c_str(), balanceBeforeStr.c_str(), 
                                   balanceAfterStr.c_str(), remark.c_str()};
    return financedb.insert("finance", columns, values);
}

vector<Finance> queryHistory(string cardId) {
    initfinance();
    
    vector<Finance> result;
    vector<const char*> params = {cardId.c_str()};
    vector<vector<string>> rows = financedb.query(
        "SELECT id, card_id, type, amount, remark, time FROM finance "
        "WHERE card_id=? ORDER BY time DESC", params);
    
    for (size_t i = 0; i < rows.size(); i++) {
        result.push_back(queryToFinance(rows, i));
    }
    return result;
}

vector<Finance> queryAllTransactions() {
    initfinance();
    
    vector<Finance> result;
    vector<vector<string>> rows = financedb.query(
        "SELECT id, card_id, type, amount, remark, time FROM finance "
        "ORDER BY time DESC");
    
    for (size_t i = 0; i < rows.size(); i++) {
        result.push_back(queryToFinance(rows, i));
    }
    return result;
}

float sumAmountByType(int type) {
    initfinance();
    
    string typeStr = to_string(type);
    vector<const char*> params = {typeStr.c_str()};
    vector<vector<string>> rows = financedb.query(
        "SELECT SUM(amount) FROM finance WHERE type=?", params);

    if (rows.empty() || rows[0][0] == "NULL") {
        return 0.0f;
    }
    try {
        return stof(rows[0][0]);
    } catch (const std::exception& e) {
        return 0.0f;
    }
}

Finance queryToFinance(const vector<vector<string>>& result, int index) {
    Finance f;
    f.id = 0;
    f.type = 0;
    f.amount = 0.0f;

    if (index >= 0 && index < (int)result.size()) {
        const auto& row = result[index];
        if (row.size() >= 6) {
            try {
                f.id = stoi(row[0]);
                f.cardId = row[1];
                f.type = stoi(row[2]);
                f.amount = stof(row[3]);
                f.remark = row[4];
                f.time = row[5];
            } catch (const std::exception& e) {
                // 数据格式错误，返回零初始化的对象
                f.id = 0;
                f.type = 0;
                f.amount = 0.0f;
            }
        }
    }
    return f;
}
