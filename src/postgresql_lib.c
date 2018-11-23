#include <string.h>
#include <libpq-fe.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "error_conn.h"
#include "utils_server.h"
#include "postgresql_lib.h"

void add_separator(char bufresponse[], int j){
  for (size_t i = 0; i < j+3; i++) {
    strcat(bufresponse,"-");
  }
}

error_connection_t query_sw_postgresql(const char bufname[],const char bufquery[], char bufresponse[]){
  PGconn *conn;
  PGresult *res;
  error_connection_t err;
  char m[100] = "[!] ERROR CONNECTION REFUSED. (Conexion rechazada, chequear parametros)";
  conn = PQsetdbLogin(PGSQL_IP,PGSQL_PORT,NULL,NULL,bufname,PGSQL_USER,PGSQL_PASS);
  if (PQstatus(conn) != CONNECTION_BAD) {
    res = PQexec(conn, bufquery);
    for (size_t i = 0; i < PQnfields(res); i++) {
      strcat(bufresponse,PQfname(res, i));
      strcat(bufresponse, SEP_COL);
    }
    strcat(bufresponse, SEP_ROW);
    for (size_t i = 0; i < PQnfields(res); i++) {
      add_separator(bufresponse, PQgetlength(res, 0, i));
      strcat(bufresponse, SEP_COL);
    }
    strcat(bufresponse, SEP_ROW);
    if (res != NULL && PGRES_TUPLES_OK == PQresultStatus(res)) {
      for (size_t i = 0 ; i <= PQntuples(res)-1;  i++) {
        for (size_t j = 0 ; j < PQnfields(res); j++) {
          strcat(bufresponse, PQgetvalue(res,i,j));
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
