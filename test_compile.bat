@echo off
echo 测试头文件引用和编译...

echo.
echo 1. 测试头文件引用...
g++ -I include -I src/sqlite3 -E src/Main.cpp -o test_output.txt >nul 2>&1
if %errorlevel% equ 0 (
    echo ✓ 头文件引用正常
) else (
    echo ✗ 头文件引用有问题
)

echo.
echo 2. 测试编译单个文件...
g++ -I include -I src/sqlite3 -c src/Main.cpp -o test_main.o >nul 2>&1
if %errorlevel% equ 0 (
    echo ✓ 单个文件编译成功
) else (
    echo ✗ 单个文件编译失败
)

echo.
echo 3. 清理测试文件...
del test_output.txt test_main.o >nul 2>&1
echo ✓ 测试完成

pause