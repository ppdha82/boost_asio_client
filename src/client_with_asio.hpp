#ifndef _BS_CLIENT_WITH_ASIO_HPP_
#define _BS_CLIENT_WITH_ASIO_HPP_

#include <string>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

using boost::asio::ip::tcp;

class client_with_asio : private boost::noncopyable
{
public:
    enum {
        max_length = 1024,
    };

    client_with_asio(const std::string& server_ip, const int server_port)
    {
        if (m_io_service.use_count() == 0) {
            m_io_service = std::make_shared<boost::asio::io_service>();
            if (m_socket.use_count() > 0 || m_resolver.use_count() > 0) {
                if (m_socket.use_count() > 0) {
                    m_socket.reset();
                    std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " reset m_socket.use_count()[" << m_socket.use_count() << "]\n";
                }
                if (m_resolver.use_count() > 0) {
                    m_resolver.reset();
                    std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " reset m_resolver.use_count()[" << m_resolver.use_count() << "]\n";
                }
            }
        }
        else {
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Already generated. m_io_service.use_count()[" << m_io_service.use_count() << "]\n";
        }
        port = server_port;
        ipAddress = server_ip;
        std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " ipAddress[" << ipAddress << "] port[" << port << "]\n";
    }

    bool isConnected()
    {
        if (m_socket.use_count() == 0 || m_resolver.use_count() == 0) {
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " reset m_socket.use_count()[" << m_socket.use_count() << "]\n";
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " reset m_resolver.use_count()[" << m_resolver.use_count() << "]\n";
            return false;
        }

        return true;
    }

    void connect()
    {
        try {
            if (m_io_service.use_count() == 0) {
                std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Failed to run " << __func__ << "m_io_service.use_count()[" << m_io_service.use_count() << "]\n";
                return;
            }
            if (m_socket.use_count() > 0 || m_resolver.use_count() > 0) {
                if (m_socket.use_count() > 0) {
                    m_socket.reset();
                    std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " reset m_socket.use_count()[" << m_socket.use_count() << "]\n";
                }
                if (m_resolver.use_count() > 0) {
                    m_resolver.reset();
                    std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " reset m_resolver.use_count()[" << m_resolver.use_count() << "]\n";
                }
            }

            m_socket = std::make_shared<tcp::socket>(*m_io_service);
            m_resolver = std::make_shared<tcp::resolver>(*m_io_service);
            char portBuffer[1024];
            itoa(port, portBuffer, 10);
            std::string cPort(portBuffer);

            boost::asio::connect(*m_socket, m_resolver->resolve({ipAddress, cPort}));
            std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] cPort[" << cPort << "] ipAddress[" << ipAddress << "]\n";
        }
        catch (std::exception& e) {
            std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] Exception: " << e.what() << "\n";
            m_socket.reset();
            m_resolver.reset();
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " reset m_socket.use_count()[" << m_socket.use_count() << "]\n";
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " reset m_resolver.use_count()[" << m_resolver.use_count() << "]\n";
        }
    }

    void sendMessage(std::string &msg, int count)
    {
        try {
            if ((m_io_service.use_count() == 0) || (m_socket.use_count() == 0) || (m_resolver.use_count() == 0)) {
                if (m_io_service.use_count() == 0) {
                    std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Failed to run " << __func__ << "m_io_service.use_count()[" << m_io_service.use_count() << "]\n";
                }
                if (m_socket.use_count() == 0) {
                    std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Failed to run " << __func__ << "m_socket.use_count()[" << m_socket.use_count() << "]\n";
                }
                if (m_resolver.use_count() == 0) {
                    std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Failed to run " << __func__ << "m_resolver.use_count()[" << m_resolver.use_count() << "]\n";
                }
                return;
            }
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Enter " << __func__ << "\n";
            char countBuffer[1024];
            std::string requestMsg;
            if (msg.empty()) {
                requestMsg = "testMessage.";
            }
            else {
                requestMsg = msg;
            }
            itoa(count, countBuffer, 10);
            requestMsg.append(countBuffer);
            size_t request_length = std::strlen(requestMsg.c_str());
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " requestMsg[" << requestMsg << "]\n";

            boost::asio::write(*m_socket, boost::asio::buffer(requestMsg, request_length));
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " requestMsg[" << requestMsg << "] request_length[" << request_length << "]\n";

            char reply[max_length];
            size_t reply_length = boost::asio::read(*m_socket, boost::asio::buffer(reply, request_length));
            std::cout << "Reply is: ";
            std::cout.write(reply, reply_length);
            std::cout << "\n";
        }
        catch (std::exception& e) {
            std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] Exception: " << e.what() << "\n";
            m_socket.reset();
            m_resolver.reset();
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " reset m_socket.use_count()[" << m_socket.use_count() << "]\n";
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " reset m_resolver.use_count()[" << m_resolver.use_count() << "]\n";
        }
    }

private:
    std::shared_ptr<boost::asio::io_service> m_io_service;
    std::shared_ptr<tcp::socket> m_socket;
    std::shared_ptr<tcp::resolver> m_resolver;
    std::string ipAddress;
    int port;
};
#endif // _BS_CLIENT_WITH_ASIO_HPP_
