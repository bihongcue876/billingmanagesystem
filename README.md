# 计费管理系统

- *(注意：这里面的主分支名为master而非main），用于习惯新操作*
- 可执行文件地址：./build/bin/AccountManagement.exe

- 头文件将参与编译，如多次调用则会编译慢速。
- 条件编译机制 #ifndef #define #endif

- 上传平台：origin
```bash
git push -u gitee master
git push -u github master
```
- 项目运行方式：run.bat , 控制台：`run`

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
```
accountmanagement/
├── src/                            # 源代码目录
│   ├── Main.cpp                    # 程序入口
│   ├── menu.cpp                    # 菜单模块实现
│   ├── service.cpp                 # 服务调度实现
│   ├── tool.cpp                    # 工具函数实现
│   ├── database.cpp                # 数据库基础操作实现
│   ├── services/                   # 业务服务模块
│   │   ├── accountdatabase.cpp     # 账户数据库操作实现
│   │   ├── accountservice.cpp      # 账户服务实现
│   │   ├── billingdatabase.cpp     # 计费标准数据库操作实现
│   │   ├── billingstandard.cpp     # 计费标准服务实现
│   │   ├── financedatabase.cpp     # 财务数据库操作实现
│   │   ├── financeservice.cpp      # 财务服务实现
│   │   ├── loginout.cpp            # 上下机服务实现
│   │   ├── loginoutdata.cpp        # 上下机数据操作实现
│   │   ├── logsearch.cpp           # 日志查询服务实现
│   │   └── search.cpp              # 搜索服务实现
│   └── sqlite3/                    # SQLite3 嵌入式数据库
│       ├── sqlite3.c
│       ├── sqlite3.h
│       ├── sqlite3ext.h
│       └── shell.c
├── include/                        # 头文件目录
│   ├── menu.h                      # 菜单接口
│   ├── service.h                   # 服务调度接口
│   ├── tool.h                      # 工具函数接口
│   ├── global.h                    # 全局变量和常量
│   ├── model.hpp                   # 数据结构定义
│   ├── database.h                  # 数据库基础操作接口
│   ├── accountdatabase.h           # 账户数据库操作接口
│   ├── accountservice.h            # 账户服务接口
│   ├── billingdatabase.h           # 计费标准数据库操作接口
│   ├── billingstandard.h           # 计费标准服务接口
│   ├── financedatabase.h           # 财务数据库操作接口
│   ├── financeservice.h            # 财务服务接口
│   ├── loginout.h                  # 上下机服务接口
│   ├── loginoutdata.h              # 上下机数据操作接口
│   ├── logsearch.h                 # 日志查询服务接口
│   └── search.h                    # 搜索服务接口
├── data/                           # 数据存储目录
│   └── data/
│       └── loginout.db             # 上下机记录数据库文件
├── build/                          # 构建输出目录
│   └── bin/
│       └── AccountManagement.exe   # 可执行文件
├── CMakeLists.txt                  # CMake 构建配置
├── README.md
├── run.bat                         # 运行脚本
└── compile.bat                     # 编译脚本
```

### UML 类图

#### 数据结构定义

```mermaid
classDiagram
    class UnitType {
        <<enumeration>>
        MINUTE
        HOUR
    }

    class Account {
        +char aName[19] 卡号
        +char aPwd[9] 密码
        +char nStatus[2] 卡状态
        +time_t tStart 开卡时间
        +time_t tEnd 截止时间
        +float fTotalUse 累计金额
        +time_t tLast 最后使用时间
        +int nUseCount 使用次数
        +float fBalance 余额
        +int nDel 删除标记
    }

    class Billing {
        +string sPackageId 套餐编号
        +UnitType nUnitType 计费单位
        +float fUnitPrice 单价
        +int nDel 是否失效
    }

    class LogInfo {
        +char aCardName[19] 上机卡号
        +time_t tStart 上机时间
        +time_t tEnd 下机时间
        +float fAmount 消费金额
        +float fBalance 余额
        +int nPackageId 套餐ID
    }
```

#### 数据库封装类

```mermaid
classDiagram
    class sqlitedb {
        -sqlite3* db 数据库连接
        -string lastError 错误信息
        +sqlitedb(dbPath) 构造函数
        +~sqlitedb() 析构函数
        +getLastError() 获取错误信息
        +exec(sql, params) 执行SQL
        +query(sql, params) 查询数据
        +tablecreate(tableName, columnDefs) 建表
        +insert(tableName, columns, values) 插入数据
        +update(tableName, setClause, whereClause, params) 更新数据
        +remove(tableName, whereClause, params) 删除数据
        -bindParams(stmt, params) 绑定参数
    }

    sqlitedb ..> Account : queryToAccount()
```

#### 模块依赖关系

```mermaid
flowchart TB
    subgraph 数据层
        DB[(SQLite3)]
        sqlitedb[sqlitedb 类]
    end

    subgraph 数据模型
        Account[Account]
        Billing[Billing]
        LogInfo[LogInfo]
        UnitType[UnitType]
    end

    subgraph 数据访问层
        accountdb[accountdatabase]
        billingdb[billingdatabase]
        financedb[financedatabase]
        loginoutdata[loginoutdata]
    end

    subgraph 服务层
        accountsvc[accountservice]
        loginout[loginout]
        finance[financeservice]
        billing[billingstandard]
        logsearch[logsearch]
        search[search]
    end

    subgraph 调度层
        service[service]
        menu[menu]
    end

    subgraph 入口
        Main[Main]
    end

    Main --> menu
    menu --> service
    service --> accountsvc
    service --> loginout
    service --> billing
    service --> finance
    service --> logsearch
    accountsvc --> accountdb
    accountdb --> sqlitedb
    billing --> billingdb
    billingdb --> sqlitedb
    finance --> financedb
    financedb --> sqlitedb
    loginout --> loginoutdata
    loginoutdata --> sqlitedb
    logsearch --> search
    search --> sqlitedb
    sqlitedb --> DB
    sqlitedb --> Account
    sqlitedb --> Billing
    sqlitedb --> LogInfo
    Billing --> UnitType
```

#### 模块说明

| 层级 | 模块 | 接口函数 | 功能 |
| --- | --- | --- | --- |
| 入口层 | Main.cpp | `main()` | 程序入口 |
| 菜单层 | menu.cpp/h | `menu()` | 主菜单 |
| 调度层 | service.cpp/h | `service1~5()` | 服务路由：账户/使用/计费/财务/查询 |
| 数据库层 | database.cpp/h | `sqlitedb` 类 | SQLite3 封装 |
| 数据访问层 | accountdatabase.cpp/h | `searchaccount()` `changeaccount()` `signup()` `deletecard()` `init()` | 账户数据操作 |
| 数据访问层 | billingdatabase.cpp/h | `newstnd()` `searchstnd()` `changestnd()` `deletestnd()` `initbilling()` `queryToBilling()` | 计费标准数据操作 |
| 数据访问层 | financedatabase.cpp/h | - | 财务数据操作 |
| 数据访问层 | loginoutdata.cpp/h | `login()` `logout()` `queryToLogInfo()` | 上下机数据操作 |
| 服务层 | accountservice.cpp/h | `accountmenu()` | 账户管理菜单 |
| 服务层 | loginout.cpp/h | `logmenu()` | 上下机菜单 |
| 服务层 | billingstandard.cpp/h | `billingmenu()` | 计费标准菜单 |
| 服务层 | financeservice.cpp/h | `financemenu()` | 财务服务菜单 |
| 服务层 | logsearch.cpp/h | `searchmenu()` | 日志查询菜单 |
| 服务层 | search.cpp/h | - | 搜索服务 |

### 信息表单（数据结构定义）

#### 1. 账户信息 (Account)

| 字段 | 类型 | 描述 |
| --- | --- | --- |
| aName | char[19] | 卡号 |
| aPwd | char[9] | 密码 |
| nStatus | char[2] | 卡状态（0-未上机，1-上机中，2-已注销，3-失效） |
| tStart | time_t | 开卡时间 |
| tEnd | time_t | 截止时间 |
| fTotalUse | float | 累计金额 |
| tLast | time_t | 最后使用时间 |
| nUseCount | int | 使用次数 |
| fBalance | float | 余额 |
| nDel | int | 删除标记：0,1 |

#### 2. 计费套餐信息 (Billing)

| 字段 | 类型 | 描述 |
| --- | --- | --- |
| sPackageId | string | 套餐编号 |
| nUnitType | UnitType | 计费单位：MINUTE-分钟，HOUR-小时 |
| fUnitPrice | float | 单价 |
| nDel | int | 是否失效：0-有效，1-失效 |

#### 3. 上下机日志信息 (LogInfo)

| 字段 | 类型 | 描述 |
| --- | --- | --- |
| aCardName | char[19] | 上机卡号 |
| tStart | time_t | 上机时间 |
| tEnd | time_t | 下机时间 |
| fAmount | float | 消费金额 |
| fBalance | float | 余额 |
| nPackageId | int | 套餐ID，0表示无套餐 |

## 编译与组织方式改动 2026年3月13日
- 构建方式：CMake
- 编译指令：
    - 初步设置：cmake -S . -B build -G "MinGW Makefiles"
    - 编译：cmake --build build
- 相关文件：CMakeLists.txt
- 生成目录：build

## 数据库详情
- 存储位置：data/data 文件夹
- 对接方式：sqlite3
- 数据库名：
    - account.db - 账户数据库
    - billing.db - 计费标准数据库
    - loginout.db - 上下机记录数据库

## 计费规则

### 套餐系统
- 默认套餐：套餐"0"，计费单位为分钟，单价0.1元/分钟
- 支持自定义套餐：可添加、查询、修改、删除计费套餐
- 套餐选择：上机时可选择套餐，直接回车使用默认套餐

### 计费取整规则

#### 分钟计费
- 上网时间 < 15分钟：向下取整
- 15分钟 ≤ 上网时间 < 60分钟：向上取整
- 上网时间 ≥ 60分钟：向下取整

#### 小时计费
- 全部向上取整

### 示例
- 上机14分钟：计费14分钟
- 上机16分钟：计费16分钟
- 上机65分钟：计费65分钟
- 上机1.2小时：计费2小时

## 其他预设
- 第一个用户账号：
    - username: 10000000
    - password: 11111111