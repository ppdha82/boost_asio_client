#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;
namespace asio = boost::asio;

int main() {
    asio::io_context io_context;

    // Server's IP address and port number
    tcp::endpoint server_endpoint(asio::ip::address::from_string("192.168.1.1"), 12345);

    // Create a socket
    tcp::socket socket(io_context);

    // Connect to the server
    socket.connect(server_endpoint);

    // Use the socket for communication...
    // For example, sending a simple message
    std::string message = "Hello, Server!";
    boost::system::error_code ignored_error;
    asio::write(socket, asio::buffer(message), ignored_error);

    // Close the socket
    socket.close();

    return 0;
}

