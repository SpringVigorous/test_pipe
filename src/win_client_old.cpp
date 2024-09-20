#include <Windows.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <common.hpp>
#include <chrono>
#include <iomanip>

using namespace std;

string now_stamp() {
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 将时间点转换为 time_t 类型
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);


    // 如果需要更高精度的时间，可以使用 chrono 的 duration
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;


    // 组合起来输出
    std::ostringstream stream;
    stream << std::put_time(std::localtime(&now_c), "%Y/%m/%d %X") << "."
        << std::setfill('0') << std::setw(3) << milliseconds.count();
    return stream.str();




}

int main() {
    std::cout << "命名管道通信客户端!\n";


    string data_pre = "客户端写入命名管道的数据:";


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
        string data = data_pre + now_stamp();

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