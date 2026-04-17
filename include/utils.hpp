#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <string>
#include <conio.h>

/**
 * @file utils.hpp
 * @brief 通用工具函数头文件
 */

/**
 * @brief 读取浮点数，带范围检查
 * @param prompt 提示信息
 * @param minVal 最小值（包含）
 * @param maxVal 最大值（包含）
 * @param minErr 最小值错误提示
 * @param maxErr 最大值错误提示
 * @return 有效的浮点数值
 */
inline float readFloat(
    const std::string& prompt,
    float minVal,
    float maxVal,
    const std::string& minErr = "数值太小！",
    const std::string& maxErr = "数值太大！"
) {
    float value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1024, '\n');
            std::cout << "输入格式错误，请重新输入！" << std::endl;
            continue;
        }
        std::cin.ignore(1024, '\n');
        if (value < minVal) {
            std::cout << minErr << std::endl;
        } else if (value > maxVal) {
            std::cout << maxErr << std::endl;
        } else {
            return value;
        }
    }
}

/**
 * @brief 读取整数，带范围检查
 * @param prompt 提示信息
 * @param minVal 最小值（包含）
 * @param maxVal 最大值（包含）
 * @param minErr 最小值错误提示
 * @param maxErr 最大值错误提示
 * @return 有效的整数值
 */
inline int readInt(
    const std::string& prompt,
    int minVal,
    int maxVal,
    const std::string& minErr = "数值太小！",
    const std::string& maxErr = "数值太大！"
) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1024, '\n');
            std::cout << "输入格式错误，请重新输入！" << std::endl;
            continue;
        }
        std::cin.ignore(1024, '\n');
        if (value < minVal) {
            std::cout << minErr << std::endl;
        } else if (value > maxVal) {
            std::cout << maxErr << std::endl;
        } else {
            return value;
        }
    }
}

/**
 * @brief 读取整数（只检查最小值）
 * @param prompt 提示信息
 * @param minVal 最小值（包含）
 * @param minErr 最小值错误提示
 * @return 有效的整数值
 */
inline int readIntMin(
    const std::string& prompt,
    int minVal,
    const std::string& minErr = "数值太小！"
) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1024, '\n');
            std::cout << "输入格式错误，请重新输入！" << std::endl;
            continue;
        }
        std::cin.ignore(1024, '\n');
        if (value < minVal) {
            std::cout << minErr << std::endl;
        } else {
            return value;
        }
    }
}

/**
 * @brief 读取整数（只检查最大值）
 * @param prompt 提示信息
 * @param maxVal 最大值（包含）
 * @param maxErr 最大值错误提示
 * @return 有效的整数值
 */
inline int readIntMax(
    const std::string& prompt,
    int maxVal,
    const std::string& maxErr = "数值太大！"
) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1024, '\n');
            std::cout << "输入格式错误，请重新输入！" << std::endl;
            continue;
        }
        std::cin.ignore(1024, '\n');
        if (value > maxVal) {
            std::cout << maxErr << std::endl;
        } else {
            return value;
        }
    }
}

/**
 * @brief 读取整数（无范围检查）
 * @param prompt 提示信息
 * @return 有效的整数值
 */
inline int readIntAny(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1024, '\n');
            std::cout << "输入格式错误，请重新输入！" << std::endl;
            continue;
        }
        std::cin.ignore(1024, '\n');
        return value;
    }
}

/**
 * @brief 读取浮点数（无范围检查）
 * @param prompt 提示信息
 * @return 有效的浮点数值
 */
inline float readFloatAny(const std::string& prompt) {
    float value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1024, '\n');
            std::cout << "输入格式错误，请重新输入！" << std::endl;
            continue;
        }
        std::cin.ignore(1024, '\n');
        return value;
    }
}

/**
 * @brief 读取密码（隐藏输入）
 * @param prompt 提示信息
 * @param maxLen 最大长度（不含 '\0'）
 * @param maskChar 掩码字符，默认为 '*'
 * @return 密码字符串
 */
inline std::string readPassword(
    const std::string& prompt,
    int maxLen,
    char maskChar = '*'
) {
    std::cout << prompt;
    std::string pwd;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!pwd.empty()) {
                pwd.pop_back();
                std::cout << "\b \b";
            }
        } else if ((int)pwd.length() < maxLen) {
            pwd.push_back(ch);
            std::cout << maskChar;
        }
    }
    std::cout << std::endl;
    return pwd;
}

/**
 * @brief 读取卡号（最多 18 位）
 * @param prompt 提示信息
 * @return 卡号字符串
 */
inline std::string readCardId(const std::string& prompt = "请输入卡号（最多 18 位）：") {
    char idn[19];
    std::cout << prompt;
    std::cin.width(19);
    std::cin >> idn;
    std::cin.ignore(1024, '\n');
    return std::string(idn);
}

/**
 * @brief 读取选项（0-9 或 '0'-'9'）
 * @param prompt 提示信息
 * @return 选项字符
 */
inline char readOption(const std::string& prompt = "输入数字指令以继续：") {
    char ch;
    std::cout << prompt;
    std::cin >> ch;
    std::cin.ignore(1024, '\n');
    return ch;
}

/**
 * @brief 格式化金额显示（保留两位小数）
 * @param amount 金额
 * @return 格式化后的字符串
 */
inline std::string formatCurrency(float amount) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << amount;
    return oss.str();
}

/**
 * @brief 显示菜单并获取选项
 * @param title 菜单标题
 * @param items 菜单项列表
 * @param showZero 是否显示"0. 退出"
 * @return 用户选择的字符
 */
inline char showMenu(
    const std::string& title,
    const std::vector<std::string>& items,
    bool showZero = true
) {
    std::cout << "\n--------" << title << "--------" << std::endl;
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << (i + 1) << ". " << items[i] << std::endl;
    }
    if (showZero) {
        std::cout << "0. 退出服务" << std::endl;
    }
    return readOption();
}

#include <fstream>
#include <cstdlib>
#include <windows.h>

inline bool ensureDirectory(const std::string& path) {
    DWORD dwAttrib = GetFileAttributesA(path.c_str());
    if(dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)){
        return true;
    }
    std::string cmd = "mkdir " + path;
    return system(cmd.c_str()) == 0;
}

inline bool saveToFile(const std::string& filepath, const std::string& content) {
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        return false;
    }
    outFile << content;
    outFile.close();
    return true;
}

#endif // UTILS_HPP
