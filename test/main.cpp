

#include <gtest/gtest.h>


using namespace std;


int main(int argc, char* argv[]) {


    // 输出格式
    // testing::GTEST_FLAG(output) = "xml:";

    // 过滤去， TEST(fs_test, framework_test)
    //   filter使用正则匹配  如: fs_tes*
    //std::string testingFilter{ "close_polyline_test*" };
    //testing::GTEST_FLAG(filter) = testingFilter;


    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}