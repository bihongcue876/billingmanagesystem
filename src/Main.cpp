#include <iostream>
#include "menu.h"
#include "admin.h"

using namespace std;

int main(void) {
    system("chcp 65001");
    cout << "欢迎访问计费查阅系统" << endl;
    int status = admin(); // 状态，如果状态为“退出”则没必要继续。
    if(status){
        menu();
    }
    return 0;
}
