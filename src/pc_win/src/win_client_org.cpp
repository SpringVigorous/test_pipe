#include <Windows.h>
#include <iostream>
#include <string>
#include <stdexcept>

#include <common.hpp>
using namespace std;


int main() {
    std::cout << "命名管道通信客户端!\n";


    const char* data = "客户端写入命名管道的数据:123456";
    const size_t data_len = strlen(data);

    // 等待命名管道
    if (!WaitNamedPipe(pipe_name, NMPWAIT_WAIT_FOREVER)) {
        CheckLastError("Failed to wait for named pipe");
    }

    std::cout << "连接管道成功\n";

    // 打开命名管道
    HANDLE hPipe = CreateFile(pipe_name,
                              GENERIC_READ | GENERIC_WRITE,
                              0,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        CheckLastError("Failed to open named pipe");
    }

    // 写入数据到管道
    DWORD bytesWritten;
    if (!WriteFile(hPipe, data, static_cast<DWORD>(data_len), &bytesWritten, NULL)) {
        CheckLastError("Failed to write to pipe");
    }

    std::cout << "写入管道成功\n";

    // 关闭管道
    CloseHandle(hPipe);

    std::cout << "按任意键继续...\n";
    std::cin.get();

    return 0;
}