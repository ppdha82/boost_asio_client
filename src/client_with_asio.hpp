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

    client_with_asio(boost::asio::io_service& _io_service, const std::string& server_ip, const int server_port)
    {
        m_io_service = std::make_shared<boost::asio::io_service>();
        port = server_port;
        ipAddress = server_ip;
        std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " ipAddress[" << ipAddress << "] port[" << port << "]\n";
    }

    void connect()
    {
        m_socket = std::make_shared<tcp::socket>(*m_io_service);
        m_resolver = std::make_shared<tcp::resolver>(*m_io_service);
        char portBuffer[1024];
        itoa(port, portBuffer, 10);
        std::string cPort(portBuffer);

        boost::asio::connect(*m_socket, m_resolver->resolve({ipAddress, cPort}));
        std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] cPort[" << cPort << "] ipAddress[" << ipAddress << "]\n";
    }

    void sendMessage(std::string &msg, int count)
    {
        try {
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
        }
    }

private:
    std::shared_ptr<boost::asio::io_service> m_io_service;
    std::shared_ptr<tcp::socket> m_socket;
    std::shared_ptr<tcp::resolver> m_resolver;
    std::string ipAddress;
    int port;
};
#if 0
class chat_client
{
public:
  chat_client(boost::asio::io_context& io_context,
      const tcp::resolver::results_type& endpoints)
    : io_context_(io_context),
      socket_(io_context)
  {
    do_connect(endpoints);
  }

  void write(const chat_message& msg)
  {
    boost::asio::post(io_context_,
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
  }

  void close()
  {
    boost::asio::post(io_context_, [this]() { socket_.close(); });
  }

private:
  void do_connect(const tcp::resolver::results_type& endpoints)
  {
    boost::asio::async_connect(socket_, endpoints,
        [this](boost::system::error_code ec, tcp::endpoint)
        {
          if (!ec)
          {
            do_read_header();
          }
        });
  }

  void do_read_header()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_read_body()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            std::cout.write(read_msg_.body(), read_msg_.body_length());
            std::cout << "\n";
            do_read_header();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_write()
  {
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            socket_.close();
          }
        });
  }

private:
  boost::asio::io_context& io_context_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

class chat_message
{
public:
  enum { header_length = 4 };
  enum { max_body_length = 512 };

  chat_message()
    : body_length_(0)
  {
  }

  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  std::size_t length() const
  {
    return header_length + body_length_;
  }

  const char* body() const
  {
    return data_ + header_length;
  }

  char* body()
  {
    return data_ + header_length;
  }

  std::size_t body_length() const
  {
    return body_length_;
  }

  void body_length(std::size_t new_length)
  {
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool decode_header()
  {
    char header[header_length + 1] = "";
    std::strncat(header, data_, header_length);
    body_length_ = std::atoi(header);
    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void encode_header()
  {
    char header[header_length + 1] = "";
    std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, header_length);
  }

private:
  char data_[header_length + max_body_length];
  std::size_t body_length_;
};
#endif
#endif // _BS_CLIENT_WITH_ASIO_HPP_
