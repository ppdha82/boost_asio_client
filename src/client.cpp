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

void init_program(char* argv[])
{
    try {
        enum { TEST_COUNT = 10, };
        std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " argv[1] [" << argv[1] << "] argv[2] [" << argv[2] << "]\n";
        std::string ipAddress(argv[1]);
        std::string cPort(argv[2]);
        int port = stoi(cPort, nullptr, 10);

        clientInstance = std::make_shared<client_with_asio>(ipAddress, port);
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

void connect()
{
    try {
        std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Enter " << __func__ << "\n";
        std::string tmpMessage("test start");

        clientInstance->connect();
        clientInstance->sendMessage(tmpMessage);
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

void do_program()
{
    try {
        enum { RETRY_CONNECT = 3, };
        int count = 0;
        int tryCount = 0;
        std::string tmpMessage("");

        do {
            std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " enter repeat count: ";
            std::cin >> count;
            if (count == 0) {
                std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Finish program\n";
                break;
            }

            if (clientInstance->isConnected() == false) {
                int reconnectCount = 0;
                std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Not connected\n";
                do {
                    reconnectCount++;
                    connect();
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Wait reconnect [" << reconnectCount << "]\n";
                } while (RETRY_CONNECT > reconnectCount && clientInstance->isConnected() == false);

                if (RETRY_CONNECT <= reconnectCount) {
                    std::cerr << "[" << __FILE__ << ":" << __LINE__ << "]" << " Failed to connect !!!!\n";
                    continue;
                }
            }

            tryCount = count;
            while (tryCount > 0) {
                clientInstance->sendMessage(tmpMessage, tryCount);
                tryCount--;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << " Wait [" << tryCount << "]\n";
            }
        } while (count > 0);
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}