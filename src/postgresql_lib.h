#ifndef postgresql_lib_h
#define postgresql_lib_h

#define PGSQL_IP   "192.168.56.102"
#define PGSQL_PORT "5432"
#define PGSQL_USER "admin"
#define PGSQL_PASS "admin"

#include "error_conn.h"

extern error_connection_t query_sw_postgresql(
  const char bufname[],const char bufquery[], char bufresponse[]);

#endif
