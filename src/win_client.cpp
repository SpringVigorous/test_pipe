#include <Windows.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <common.hpp>
#include <format>
#include <sstream>

using namespace std;


int main() {
    std::cout << "命名管道通信客户端!\n";





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

    HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, event_name);
    if (hEvent == NULL) {
        CheckLastError("Failed to open event");
    }
    bool is_first = true;
    while (true)
    {
        // 写入数据到管道
        DWORD bytesWritten;
        string input_data;
         std::getline(std::cin, input_data);
        // cin >> input_data;

        

        string data =std::format("{} {}", input_data , now_stamp());
       

        if (!WriteFile(hPipe, data.c_str(), static_cast<DWORD>(data.length()), &bytesWritten, NULL)) {
            CheckLastError("Failed to write to pipe");
        }

        std::cout << "写入管道成功:"<< data<<std::endl;
        // 设置事件

        if (!SetEvent(hEvent)) {
            CheckLastError("SetEvent failed");
        }



        // 暂停一段时间
        this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 关闭管道
    CloseHandle(hPipe);
    // 关闭事件对象
    CloseHandle(hEvent);
    std::cout << "按任意键继续...\n";
    std::cin.get();

    return 0;
}