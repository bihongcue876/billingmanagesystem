@echo off
echo ========================================
echo   计费管理系统 - 快速编译脚本
echo ========================================
echo.

REM 检查 CMake 是否安装
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未检测到 CMake，请先安装 CMake
    pause
    exit /b 1
)

REM 检查 MinGW 是否安装
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未检测到 MinGW GCC 编译器
    pause
    exit /b 1
)

echo [1/3] 清理旧的构建目录...
if exist build (
    rmdir /s /q build
)

echo [2/3] 生成构建文件...
cmake -S . -B build -G "MinGW Makefiles"
if %ERRORLEVEL% NEQ 0 (
    echo [错误] CMake 配置失败
    pause
    exit /b 1
)

echo [3/3] 编译项目...
cmake --build build
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 编译失败
    pause
    exit /b 1
)

echo.
echo ========================================
echo   编译成功！可运行 app.exe
echo ========================================
pause
