#ifndef BILLINGDATABASE_H
#define BILLINGDATABASE_H
#include <iostream>
#include <string>
#include "model.hpp"
#include "database.h"

extern sqlitedb billingdb;

void initbilling();

bool newstnd();

bool searchstnd(const std::string& id = "");

bool changestnd(const std::string& id);

bool deletestnd(const std::string& id);

Billing queryToBilling(const std::vector<std::vector<std::string>>& result, int index); // 询问转记录结构体

#endif