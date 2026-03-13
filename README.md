# 计费管理系统

- *(注意：这里面的主分支名为master而非main)，用于习惯新操作*
- 注意：头文件将参与编译，如多次调用则会编译慢速。
- 条件编译机制 #ifndef #define #endif

## 前序安排
- 使用compile.bat快速编译，即注意在cmd终端里面编译和运行。

## 需求分析
- 设计计费基于控制台交互的管理系统，包含三个主要需求
    - 1. 基于卡号的用户管理制度
    - 2. 用户有关数据管理
    - 3. 上下机活动和结算

## CLI菜单设计
- 封装函数。
- 一个菜单不超过6项，通常为5项，编号不超过5
> 心理学中的 “米勒定律”（Miller's Law），即人类的工作记忆容量大约为 7±2 个组块。这一原则常被引申到用户界面设计中，建议菜单或导航的选项数量控制在 5～9 项 以内，以避免用户因信息过载而产生认知负担。

## 项目设计
### 功能设计

### 项目结构
- 服务本体
    - main.cpp
    - menu.cpp/menu.h
        - 对main接口 menu()
    - global.h 全局变量和常量
    - tool.cpp/tool.h 通用工具文件
    - model.hpp 数据结构
    - 服务
        - service.cpp/service.h
            - 对menu接口 service1() service2() service3() service4() service5()
    - 数据库和数据服务
        - 
    1. 账户相关 --> 对应card_service.h
        - accountservice.cpp/accountservice.h
            - 对service接口 accountmenu()
            - 功能：
                - void accountsearch(void);// 查询账户
                - void accountchange(void);// 账户信息变更
                - void signupaccount(void);// 注册账户
                - void deleteaccount(void);// 注销账户

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

### 信息表单（结构体数组设计卡务）
1. 卡务信息

2. 计费信息

3. 计费标准信息

4. 充值退费信息

5. 管理员