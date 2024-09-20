
#include <format>
#include <string>
#include <chrono>
#include <iomanip>
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


inline const LPCSTR pipe_name = R"(\\.\pipe\pipeName)";
inline const size_t buffer_size = 4096;
inline LPCSTR event_name = "Global\\MyEvent"; 

