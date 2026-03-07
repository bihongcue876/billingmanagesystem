# 计费管理系统

- (注意：这里面的主分支名为master而非main)

## 前序安排
- 使用compile.bat快速编译，即注意在cmd终端里面编译和运行。

## 需求分析
- 设计计费基于控制台交互的管理系统，包含三个主要需求
    - 1. 基于卡号的用户管理制度
    - 2. 用户有关数据管理
    - 3. 上下机活动和结算

## 项目设计
- 服务本体
    - main.cpp
    - menu.cpp/menu.h
        - 对main接口 menu()
    - 服务
        - service.cpp/service.h
            - 对menu接口 service1() service2() service3() service4() service5()
    1. 账户相关
        - accountservice.cpp/accountservice.h
            - 对service接口 accountmenu()
            - 功能：
                - void accountsearch(void);// 查询账户
                - void accountchange(void);// 账户信息变更
                - void signupaccount(void);// 注册账户
                - void deleteaccount(void);// 删除账户
    2. 上机下机
        - loginout.cpp/loginout.h
            - 对service接口 logmenu()
            - 功能：
                - void login(void);// 上机
                - void logout(void);// 下机
    3. 计费标准设定
        - billingstandard.cpp/billingstandard.h
            - 对service接口 billingmenu()
            - 功能：
                - void newstandard(void);// 新增计费标准
                - void searchstandard(void);// 查询计费标准
                - void changestandard(void);// 改动计费标准
                - void deletestandard(void);// 删除计费标准
    4. 财务系统
        - financeservice.cpp/financeservice.h
            - 对service接口 financemenu()
            - 功能：
                - void newstandard(void);// 新增计费标准
                - void searchstandard(void);// 查询计费标准
                - void changestandard(void);// 改动计费标准
                - void deletestandard(void);// 删除计费标准
    5. 查询统计
        - logsearch.cpp/logsearch.h
            - 对service接口 searchmenu()
            - 功能：
                - void totalsales(void);// 营业额与日志
                - void uselogs(void);// 上下机日志
                - void logprint(void);// 日志打印

