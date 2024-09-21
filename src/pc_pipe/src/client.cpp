// client.cpp
#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace boost::asio;
using namespace std;

int main() {
    io_context io_context;

    // 定义命名管道名称
    const string pipe_name_recv = R"(\\.\pipe\mypiperecv)";
    const string pipe_name_send = R"(\\.\pipe\mypipesend)";

    // 创建接收管道
    local::stream_protocol::socket socket_recv(io_context);
    socket_recv.connect(local::stream_protocol::endpoint(pipe_name_recv));

    // 创建发送管道
    local::stream_protocol::socket socket_send(io_context);
    socket_send.connect(local::stream_protocol::endpoint(pipe_name_send));

    char buffer[100];
    for (;;) {
        // 向发送管道写入数据
        string message = "Client says: Hi!";
        socket_send.write_some(boost::asio::buffer(message.data(), message.size()));

        // 从接收管道读取数据
        streamsize length = socket_recv.read_some(boost::asio::buffer(buffer, sizeof(buffer)));
        buffer[length] = '\0';  // 确保字符串以null结尾
        cout << "Client received: " << buffer << endl;
    }

    return 0;
}