#ifndef BILLINGDATABASE_H
#define BILLINGDATABASE_H
#include "model.hpp"

void newstnd(void);

void searchstnd(void);

void changestnd(void);

void deletestnd(void);

Billing queryToBilling(const std::vector<std::vector<std::string>>& result, int index = 0); // 询问转记录结构体

#endif