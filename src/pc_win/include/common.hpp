
#include <format>
#include <string>
#include <chrono>
#include <iomanip>
#include <Windows.h>

inline void CheckLastError(const std::string& message) {
    DWORD error = GetLastError();
    if (error != ERROR_SUCCESS) {
        throw std::runtime_error(std::format(std::string("{}:{}"), message ,error));
    }
}


inline std::string now_stamp() {
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

//废弃
inline bool createDetachedProcess(const std::string& executablePath, const std::string& arguments = "")
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

inline const LPCSTR pipe_name = R"(\\.\pipe\pipeName)";
inline const size_t buffer_size = 4096;
inline LPCSTR event_name = "Global\\MyEvent"; 

