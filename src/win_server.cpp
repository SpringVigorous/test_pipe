#include <Windows.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <common.hpp>
using namespace std;

// 完成例程
static VOID WINAPI CompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransferred, LPOVERLAPPED lpOverlapped) {
    // 更新实际读取的数据大小
    OVERLAPPED* pOverlapped = reinterpret_cast<OVERLAPPED*>(lpOverlapped);

    // 获取实际读取的数据大小
    DWORD bytesRead = 0;
    GetOverlappedResult(pOverlapped->hEvent, pOverlapped, &bytesRead, FALSE);

    // 在这里可以处理读取完成后的数据
    cout << "完成例程调用，读取字节数: " << bytesRead << endl;
}

bool createDetachedProcess(const std::string& executablePath, const std::string& arguments = "")
{
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));


    // 创建一个新的控制台窗口
     //AllocConsole();

     // 获取新的控制台窗口的句柄
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // 设置 STARTUPINFO 结构体
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    //si.hStdInput = hConsole;
    //si.hStdOutput = hConsole;
    //si.hStdError = hConsole;
    si.wShowWindow = SW_SHOW;




    // 设置命令行参数
    std::string commandLine = arguments.empty() ? executablePath : executablePath + " " + arguments;
    // 设置创建标志
    //DWORD creationFlags = CREATE_NEW_CONSOLE | DETACHED_PROCESS;
    DWORD creationFlags = CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS;
    // 调用 CreateProcess
    if (!CreateProcess(
        executablePath.c_str(),  // 可执行文件路径
        (LPSTR)arguments.c_str(),  // 命令行参数
        //nullptr,  // 命令行参数
        nullptr,  // 进程安全属性

        nullptr,  // 不继承线程句柄5
        FALSE,    // 不继承句柄
        creationFlags,        // 默认创建标志
        nullptr,  // 使用父进程环境
        nullptr,  // 使用父进程当前目录
        &si,      // 启动信息
        &pi       // 进程信息
    ))
    {
        DWORD error = GetLastError();
        std::cerr << "CreateProcess failed with error code: " << error << std::endl;
        return false;
    }
    //WaitForSingleObject(pi.hProcess, INFINITE);



    // 关闭进程和线程句柄
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}
void create_client()
{
    // 指定要启动的可执行文件路径
    std::string executablePath = R"(F:\test\test_pipe\build\debug\bin\win_client_d.exe)";  // 替换为实际路径
    std::string arguments = "";  // 可选参数

    if (createDetachedProcess(executablePath, arguments))
    {
        std::cout << "子进程启动成功。\n";
    }
    else
    {
        std::cout << "子进程启动失败。\n";
    }
}
int main() {


    // 创建 OVERLAPPED 结构
    OVERLAPPED overlapped = {};
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, event_name);

    std::cout << "命名管道通信服务端!\n";

    // 创建命名管道
    HANDLE pipe = CreateNamedPipe(
        pipe_name,
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        buffer_size,  // 输出缓冲区大小
        buffer_size,  // 输入缓冲区大小
        NMPWAIT_USE_DEFAULT_WAIT,     // 客户端超时时间
        NULL   // 安全描述符
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        CheckLastError("Failed to create named pipe");
    }

    std::cout << "命名管道创建成功\n";
    create_client();
    // 连接命名管道
    if (!ConnectNamedPipe(pipe, NULL)) {
        DWORD error = GetLastError();
        if (error != ERROR_PIPE_CONNECTED) {
            CheckLastError("Failed to connect named pipe");
        }
    }

    std::cout << "命名管道客户端已连接\n";






    DWORD len{ 0 };
    char buf[buffer_size];
    cout << "缓冲区长度：" << buffer_size << endl;



    // 主循环
    while (true) {

        // 发起异步读取
        if (!ReadFileEx(pipe, buf, buffer_size, &overlapped, CompletionRoutine)) {
            if (GetLastError() != ERROR_IO_PENDING) {
                CheckLastError("Failed to initiate async read");
            }
        }

        // 等待事件
        DWORD result = WaitForSingleObject(overlapped.hEvent, INFINITE);
        if (result == WAIT_OBJECT_0) {
            DWORD bytesRead = 0;
            GetOverlappedResult(pipe, &overlapped, &bytesRead, FALSE);

            std::cout << now_stamp() << " 接收到数据: " << std::string(buf, bytesRead) << "\n";

            // 清除事件状态
            if (!ResetEvent(overlapped.hEvent)) {
                CheckLastError("ResetEvent failed");
            }
        }
    }

    // // 读取数据
    // if (!ReadFile(pipe, buf, buffer_size, &len, NULL)) {
    //     CheckLastError("Failed to read from pipe");
    // }
    // string content(buf, len);
    // std::cout << content << endl;

    // 关闭管道
    CloseHandle(pipe);
    CloseHandle(overlapped.hEvent);

    std::cout << "按任意键继续...\n";
    std::cin.get();

    return 0;
}