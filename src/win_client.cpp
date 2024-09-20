#include <Windows.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <common.hpp>
#include <format>
#include <sstream>
#include <algorithm>
#include <iterator>
using namespace std;
using namespace std::chrono_literals;

std::string trim(const std::string& str)
{
    auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
    auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();

    return (start < end) ? std::string(start, end) : std::string();
}

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
        //  std::getline(std::cin, input_data);
        cin >> input_data;
        if (trim(input_data).empty()) {
            continue;
        }
        

        string data =std::format("{} {}", input_data , now_stamp());
       

        if (!WriteFile(hPipe, data.c_str(), static_cast<DWORD>(data.length()), &bytesWritten, NULL)) {
         DWORD lastError = GetLastError();
            if (lastError == ERROR_BROKEN_PIPE or lastError== ERROR_NO_DATA) {
                std::cerr << "服务端已关闭管道\n";
                break;  // 退出循环
            } else {
                CheckLastError("Failed to write to pipe");
            }
        }

        std::cout << "写入管道成功:"<< data<<std::endl;
        // 设置事件

        if (!SetEvent(hEvent)) {
            CheckLastError("SetEvent failed");
        }



        // 暂停一段时间
        this_thread::sleep_for(500ms);
    }

    // 关闭管道
    CloseHandle(hPipe);
    // 关闭事件对象
    CloseHandle(hEvent);
    std::cout << "按任意键继续...\n";
    std::cin.get();

    return 0;
}