#include <Windows.h>
#include <iostream>
#include <stdexcept>

#include <common.hpp>
using namespace std;


int main() {
    std::cout << "命名管道通信服务端!\n";

    // 创建命名管道
    HANDLE pipe = CreateNamedPipe(
        pipe_name,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        4096,  // 输出缓冲区大小
        4096,  // 输入缓冲区大小
        0,     // 客户端超时时间
        NULL   // 安全描述符
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        CheckLastError("Failed to create named pipe");
    }

    OVERLAPPED overlapped = { 0 };

    // 连接命名管道
    if (!ConnectNamedPipe(pipe, &overlapped)) {
        DWORD error = GetLastError();
        if (error != ERROR_PIPE_CONNECTED) {
            CheckLastError("Failed to connect named pipe");
        }
    }

    std::cout << "命名管道客户端已连接\n";

    DWORD len;
    char buf[4096];

    // 读取数据
    if (!ReadFile(pipe, buf, 4096, &len, NULL)) {
        CheckLastError("Failed to read from pipe");
    }
    string content(buf, len);
    std::cout << content << endl;

    // 关闭管道
    CloseHandle(pipe);

    std::cout << "按任意键继续...\n";
    std::cin.get();

    return 0;
}