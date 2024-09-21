#pragma once
#include <stdint.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/process/extend.hpp>
#include <chrono>
#if defined(BOOST_WINDOWS_API)
#include <TlHelp32.h>
#include <boost/locale.hpp>
#elif defined(BOOST_POSIX_API)
#include <signal.h>
#include <sys/types.h>
#endif
#include "process_tools/process_helper.h"
_PPROCESS_TOOLS_BEGIN_
uint32_t cur_process_id() {
#ifdef _WIN32
    return GetCurrentProcessId();
#else
    return ::getpid();
#endif
}
std::string cur_time() {
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
int is_running(int pid) {
#if defined(BOOST_WINDOWS_API)
    HANDLE pss = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(pe);
    if (Process32First(pss, &pe)) {
        do {
            // pe.szExeFile can also be useful
            if (pe.th32ProcessID == pid) return 0;
        } while (Process32Next(pss, &pe));
    }
    CloseHandle(pss);
    return -1;
#elif defined(BOOST_POSIX_API)
    // 0 : running, -1 : exit, -2 : zombie
    if (0 == kill(pid, 0)) {
        std::string path = std::string("/proc/") + std::to_string(pid) + "/status";
        std::ifstream fs;
        fs.open(path);
        if (!fs) return -1;
        std::string line;
        while (getline(fs, line)) {
            std::size_t found = line.find("State:");
            if (found == std::string::npos)
                continue;
            else {
                found = line.find("zombie");
                if (found == std::string::npos)
                    return 0;
                else
                    return -2;  // zombie
            }
        }
    }
    else
        return -1;
#endif
}
std::tuple<bool, std::string> kill_child_process(int pid) {
    std::string err;
    bool ret = false;
    try {
        auto id = boost::process::pid_t(pid);
        boost::process::child pros(id);
        std::error_code ec;
        pros.terminate(ec);
        if (ec.value() == 0) return std::make_tuple(true, err);
    }
    catch (boost::process::process_error& exc) {
        err = exc.what();
    }
    return std::make_tuple(false, err);
}
std::tuple<bool, std::string> kill_process(int pid, bool isChild) {
    std::string err;
    bool ret = false;
#if defined(BOOST_WINDOWS_API)
    return kill_child_process(pid);
#elif defined(BOOST_POSIX_API)
    if (isChild)
        return killChildProcess(pid);
    else  // if not a child process,will not kill the process correctly
    {
        std::string cmd("kill -9 ");
        cmd += std::to_string(pid);
        auto ret = boost::process::system(cmd);
        if (ret == 0) return std::make_tuple(true, err);
    }
    return std::make_tuple(false, err);
#endif
}
std::tuple<std::vector<int>, std::string> get_process_pids_by_name(const std::string& processName) {
    std::vector<int> pids;
    std::string err;
    try {
#if defined(BOOST_WINDOWS_API)
        std::wstring wName;

        wName = boost::locale::conv::to_utf<wchar_t>(processName , "GBK");
        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32W entry;
        entry.dwSize = sizeof entry;
        if (!Process32FirstW(hSnapShot, &entry)) {
            return std::make_tuple(std::move(pids), err);
        }
        do {
            if (std::wstring(entry.szExeFile) == wName) {
                pids.emplace_back(entry.th32ProcessID);
            }
        } while (Process32NextW(hSnapShot, &entry));
#elif defined(BOOST_POSIX_API)
        boost::filesystem::path path = "/proc";
        boost::filesystem::directory_iterator end;
        for (boost::filesystem::directory_iterator iter(path); iter != end; iter++) {
            boost::filesystem::path p = *iter;
            std::ifstream statusFile;
            statusFile.open(p.string() + std::string("/status"));
            if (!statusFile) continue;
            std::string statusContent;
            getline(statusFile, statusContent);
            std::vector<std::string> a;
            boost::algorithm::split(a, statusContent, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
            if (boost::algorithm::trim_copy(a[1]) == processName) {
                pids.push_back(std::stoi(p.leaf().string()));
            }
            statusFile.close();
            statusFile.clear();
        }
#endif
    }
    catch (boost::process::process_error& exc) {
        err = exc.what();
    }
    return std::make_tuple(std::move(pids), err);
}
std::tuple<int, std::string> start_process(const std::string& processName, const std::string& processArgv) {
    int pid = -1;
    std::string err;
    try {
#if defined(BOOST_WINDOWS_API)
        auto p = processName /*+ ".exe"*/;
        if (!boost::filesystem::exists(p)) {
            p = boost::filesystem::current_path().string() + "/" + p;
            if (!boost::filesystem::exists(p)) {
                err = "process not exist";
                return std::make_tuple(pid, err);
            }
        }
        boost::process::child c(
            p, processArgv, boost::process::extend::on_setup = [](auto& exec) {
                exec.creation_flags |= boost::winapi::CREATE_NEW_CONSOLE_;
            });
#elif defined(BOOST_POSIX_API)
        auto p = processName;
        p = boost::filesystem::current_path().string() + "/" + p;
        if (!boost::filesystem::exists(p)) {
            err = "process not exist";
            return std::make_tuple(pid, err);
        }
        p = p + " " + processArgv;
        boost::process::child c(p);
#endif
        pid = c.id();
        // detach as a single process
        c.detach();
    }
    catch (boost::process::process_error& exc) {
        err = exc.what();
        pid = -1;
    }
    return std::make_tuple(pid, err);
}
_PPROCESS_TOOLS_END_
