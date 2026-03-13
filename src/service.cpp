#include <iostream>
#include "service.h"
#include "accountservice.h"
#include "billingstandard.h"
#include "financeservice.h"
#include "loginout.h"
#include "logsearch.h"

using namespace std;

void service1(){
    accountmenu();
}
void service2(){
    logmenu();
}
void service3(){
    billingmenu();
}
void service4(){
    financemenu();
}
void service5(){
    searchmenu();
}