#ifndef BILLINGDATABASE_H
#define BILLINGDATABASE_H
#include <iostream>
#include <string>
#include "model.hpp"

bool newstnd();

bool searchstnd();

bool changestnd();

bool deletestnd();

Billing queryToBilling(const std::vector<std::vector<std::string>>& result, int index); // 询问转记录结构体

#endif