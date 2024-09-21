
#include "gtest/gtest.h"
#include "process_tools/process_helper.h"
#include <format> 
TEST(AsioExample, TestProcessTools) {
     std::string processArgv = R"(C:\Users\Administrator\Desktop\vcpkg_export.txt)";
     std::string processName ="notepad.exe";
     std::string process_path =std::format(R"({}/{})", R"(C:\Windows\System32)",processName);
     // 启动进程
     auto newtup = PPROCESS_TOOLS::start_process(process_path, processArgv);
     auto pid = std::get<0>(newtup);

     // 查询进程是否在运行
     if (0 != PPROCESS_TOOLS::is_running(pid)) {
         std::cerr << "process " << pid << " is not running!" << std::endl;
         return;
     }
     else {
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


     EXPECT_EQ(1 + 1, 2);
 }

