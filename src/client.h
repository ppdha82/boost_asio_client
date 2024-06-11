#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "client_with_asio.hpp"

void init_program(char* argv[]);
void connect();
void do_program();

#endif  /* __CLIENT_H__ */