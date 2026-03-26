#ifndef FINANCEDATABASE_H
#define FINANCEDATABASE_H

#include <string>
#include <vector>
#include "database.h"
#include "model.hpp"

using namespace std;

extern sqlitedb financedb;

void initfinance();

bool insertTransaction(string cardId, int type, float amount,
                       float balanceBefore, float balanceAfter,
                       string remark = "");

vector<Finance> queryHistory(string cardId);

vector<Finance> queryAllTransactions();

float sumAmountByType(int type);

Finance queryToFinance(const vector<vector<string>>& result, int index);

#endif
