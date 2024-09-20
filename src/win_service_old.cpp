#include <Windows.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <common.hpp>
using namespace std;

// 完成例程
void CompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode != 0) {
        CheckLastError("Error in completion routine");
    }

    // 重新发起读取操作
    ReadFileEx(reinterpret_cast<HANDLE>(lpOverlapped->hEvent), nullptr, 0, lpOverlapped, CompletionRoutine);
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
            std::cout << "接收到数据: " << buf << "\n";
        }
        if (!ResetEvent(overlapped.hEvent)) {
            CheckLastError("ResetEvent failed");

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