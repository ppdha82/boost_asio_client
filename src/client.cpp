//
// blocking_tcp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>

#include <boost/asio.hpp>

#include "client.h"

using boost::asio::ip::tcp;

enum { max_length = 1024 };

std::shared_ptr<client_with_asio> clientInstance;

void connect(char* argv[])
{
    try {
        enum { TEST_COUNT = 10, };
        int count = 0;
        std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " argv[1] [" << argv[1] << "] argv[2] [" << argv[2] << "]\n";
        std::string ipAddress(argv[1]);
        std::string cPort(argv[2]);
        int port = stoi(cPort, nullptr, 10);
        boost::asio::io_service io_service;
        std::string tmpMessage("test start");

        clientInstance = std::make_shared<client_with_asio>(io_service, ipAddress, port);
        clientInstance->connect();
        clientInstance->sendMessage(tmpMessage, 0);

        tmpMessage.clear();
        while (count < TEST_COUNT) {
            clientInstance->sendMessage(tmpMessage, count);
            count++;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Wait [" << count << "]\n";
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

