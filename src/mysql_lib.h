#ifndef mysql_lib_h
#define mysql_lib_h

#define MYSQL_IP "192.168.56.102"
#define MYSQL_PORT "3306"
#define MYSQL_USER "admin"
#define MYSQL_PASS "admin"
#include "error_conn.h"

error_connection_t query_sw_mysql(
  const char bufname[],const char bufquery[], char bufresponse[]);

#endif
