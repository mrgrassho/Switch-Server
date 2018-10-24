#include <string.h>
#include <libpq-fe.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "switch_server.h"
#include "error_conn.h"

#define PGSQL_IP   "127.0.0.1"
#define PGSQL_PORT "5432"
#define PGSQL_USER "admin"
#define PGSQL_PASS "admin"


error_connection_t query_sw_postgresql(const char bufname[],const char bufquery[], char bufresponse[]){
  PGconn *conn;
  PGresult *res;
  error_connection_t err;
  char m[100] = "[!] ERROR CONNECTION REFUSED. (Conexion rechazada, chequear parametros)";
  conn = PQsetdbLogin(PGSQL_IP,PGSQL_PORT,NULL,NULL,bufname,PGSQL_USER,PGSQL_PASS);
  if (PQstatus(conn) != CONNECTION_BAD) {
   res = PQexec(conn, bufquery);
    if (res != NULL && PGRES_TUPLES_OK == PQresultStatus(res)) {
      for (size_t i = 0 ; i <= PQntuples(res)-1;  i++) {
        for (size_t j = 0 ; j < PQnfields(res); j++) {
          strcat(bufresponse,PQgetvalue(res,i,j));
          strcat(bufresponse, SEP_COL);
        }
        strcat(bufresponse, SEP_ROW);
     }
     strcat(bufresponse,"\0");
     PQclear(res);
    }
  } else {
    goto error_stm;
  }
  PQfinish(conn);
  err.err_code = CONN_SUCCEED;
  return err;
error_stm:
  memcpy(err.msj, m, sizeof(m));
  err.err_code = CONN_REFUSED;
  return err;
}
