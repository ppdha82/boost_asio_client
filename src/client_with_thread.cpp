#include "stdafx.h"


#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define _BUFF_SIZE 128 
#define _MY_IP "192.168.25.22" 

using boost::asio::ip::tcp; 

class CProtocol 
{ 
private: 
	tcp::socket m_Socket; 
	bool m_bConnect; 

	int m_nTestCount; 

public: 

	CProtocol(boost::asio::io_service& io) : m_Socket(io) 
	{ 
		m_bConnect = false; 
		m_nTestCount = 0; 
	} 

	~CProtocol() {} 

	void Connect() 
	{ 
		// 입력 받은 host을 resolving한다. 
		tcp::resolver resolver(m_Socket.get_io_service()); 
		tcp::resolver::query query(_MY_IP, "daytime"); 
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query); 
		tcp::resolver::iterator end; 

		// resolving된 endpoint로 접속을 시도한다. 
		boost::system::error_code error = boost::asio::error::host_not_found; 
		while (error && endpoint_iterator != end) 
		{ 
			m_Socket.close(); 
			m_Socket.connect(*endpoint_iterator++, error); 
		} 

		// 접속 실패인지 확인 
		if (error) 
			throw boost::system::system_error(error); 

		m_bConnect = true; 

		// 읽어올 데이터를 저장할 array를 만든다. 
		boost::array<CHAR, _BUFF_SIZE> buf; 
		size_t len = m_Socket.read_some(boost::asio::buffer(buf), error); 
		if (error == boost::asio::error::eof) 
		{ 
			m_bConnect = false; 
			return; 
		} 
		else if (error) 
			throw boost::system::system_error(error); 

		// 받은 데이터를 cout로 출력한다. 
		std::cout.write(buf.data(), len); 
	} 

	bool IsRun() { return m_bConnect; } 
	bool IsSocketOpen() 
	{ 
		if(!m_Socket.is_open() && m_bConnect)   // 커넥트 된 이후 소켓이 닫혀버렸다면 
		{ 
			m_bConnect = false;                 // 커넥트도 끊김 판정 
			return false; 
		} 

		return true; 
	} 

	void handle_Recive() 
	{ 

	} 

	void handle_Send() 
	{ 
		while(m_bConnect) 
		{ 
			if(!IsSocketOpen()) 
				break; 

			try
			{ 
				boost::array<BYTE, _BUFF_SIZE> buf = { boost::lexical_cast<BYTE>(m_nTestCount) }; 
				boost::system::error_code error; 
				int len = boost::asio::write(m_Socket, boost::asio::buffer(buf, buf.size()), error); 
				if(len > 0) 
					std::cout << "> Send " << m_nTestCount << std::endl; 
				m_nTestCount++;  
			} 
			catch (std::exception& e) 
			{ 
				m_bConnect = false; 
				std::cerr << e.what() << std::endl; 
			} 

			Sleep(3000); 
		} 
	} 
}; 

int _tmain(int argc, char* argv[]) 
{ 
	try
	{ 
		boost::asio::io_service io_service; // io_service를 하나 생성한다. 

		CProtocol Ptc(io_service); 
		boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service)); 
		Ptc.Connect(); // 접속 시도 
		boost::thread Recv(boost::bind(&CProtocol::handle_Recive, &Ptc)); 
		boost::thread Send(boost::bind(&CProtocol::handle_Send, &Ptc)); 
		io_service.run(); 

		while(Ptc.IsRun()) 
		{ 
		} 

		Recv.join(); 
		Send.join(); 

		t.join();   // 쓰레드가 종료될 때까지 메인 함수의 종료를 막는다 
	} 
	catch (std::exception& e) 
	{ 
		std::cerr << e.what() << std::endl; 
	} 

	int in; 
	std::cout << "END"; 
	std::cin >> in ; 

	return 0; 
}