// server.cpp
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
    local::stream_protocol::acceptor acceptor_recv(io_context, local::stream_protocol::endpoint(pipe_name_recv));
    local::stream_protocol::socket socket_recv(io_context);

    // 接受连接
    acceptor_recv.accept(socket_recv);

    // 创建发送管道
    local::stream_protocol::acceptor acceptor_send(io_context, local::stream_protocol::endpoint(pipe_name_send));
    local::stream_protocol::socket socket_send(io_context);

    // 接受连接
    acceptor_send.accept(socket_send);

    char buffer[100];
    for (;;) {
        // 从接收管道读取数据
        streamsize length = socket_recv.read_some(boost::asio::buffer(buffer, sizeof(buffer)));
        buffer[length] = '\0';  // 确保字符串以null结尾
        cout << "Server received: " << buffer << endl;

        // 向发送管道写入数据
        string response = "Server says: Hello!";
        socket_send.write_some(boost::asio::buffer(response.data(), response.size()));
    }

    return 0;
}