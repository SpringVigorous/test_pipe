#pragma once
#include <tuple>
#include <string>
#include "process_tools/macro.h"

_PPROCESS_TOOLS_BEGIN_

PPROCESS_TOOLS_API std::string cur_time();

PPROCESS_TOOLS_API int is_running(int pid);
PPROCESS_TOOLS_API std::tuple<bool, std::string> kill_child_process(int pid);
PPROCESS_TOOLS_API std::tuple<bool, std::string> kill_process(int pid, bool isChild = true);
PPROCESS_TOOLS_API std::tuple<int, std::string> start_process(const std::string& processName, const std::string& processArgv);
PPROCESS_TOOLS_API std::tuple<std::vector<int>, std::string> get_process_pids_by_name(const std::string& processName);
_PPROCESS_TOOLS_END_


/*用法示例
#include "process_helper.h"

int main() {
  std::string processArgv = "test.txt";
  std::string processName = "notepad";
  // 启动进程
  auto newtup = PPROCESS_TOOLS::start_process(processName, processArgv);
  auto pid = std::get<0>(newtup);

  // 查询进程是否在运行
  if (0 != PPROCESS_TOOLS::is_running(pid)) {
    std::cerr << "process " << pid << " is not running!" << std::endl;
    return -1;
  } else {
    std::cout << "process " << pid << " is running!" << std::endl;
  }

  // 根据进程名查询所有的pids
  auto tup = PPROCESS_TOOLS::get_process_pids_by_name(processName);
  auto pids = std::get<0>(tup);

  // 杀掉所有的同名进程
  for (auto pid : pids) {
    std::cout << "killing " << pid << std::endl;
    PPROCESS_TOOLS::kill_process(pid, false);
    if (0 != PPROCESS_TOOLS::is_running(pid)) std::cout << "process " << pid << " killed!" << std::endl;
  }
  return 0;
}

*/