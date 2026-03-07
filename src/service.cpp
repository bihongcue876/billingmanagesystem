#include <iostream>
#include "headfile/service.h"
#include "headfile/accountservice.h"
#include "headfile/billingstandard.h"
#include "headfile/financeservice.h"
#include "headfile/loginout.h"
#include "headfile/logsearch.h"

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