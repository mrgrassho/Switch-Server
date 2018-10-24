
#include "mysql.h"// REVISARR

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "error_conn.h"
#include "switch_server.h"


#define MYSQL_IP "192.168.1.40"
#define MYSQL_PORT "3306"
#define MYSQL_USER "admin"
#define MYSQL_PASS "admin"


error_connection_t query_sw_mysql(char bufname[],char bufquery[],char bufresponse[])
{
  MYSQL *conn; /* variable de conexi�n para MySQL */
  MYSQL_RES *res; /* variable que contendra el resultado de la consuta */
  MYSQL_ROW row; /* variable que contendra los campos por cada registro consultado */
  char *server = MYSQL_IP ; /*direccion del servidor 127.0.0.1, localhost o direccion ip */
  char *user = MYSQL_USER ; /*usuario para consultar la base de datos */
  char *password = MYSQL_PASS; /* contrase�a para el usuario  */
  char *database = bufname; /*nombre de la base de datos a consultar */
  conn = mysql_init(NULL); /*inicializacion a nula la conexi�n */
  error_connection_t * err;
  unsigned int num_fields;
	/* conectar a la base de datos */
	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
	  goto errr_stm;
	}

  if (mysql_query(conn, bufquery)) {
		goto errr_stm;
	}

  memset(bufresponse,0,1024);
  res = mysql_use_result(conn);
  num_fields = mysql_num_fields(res);
	while ((row = mysql_fetch_row(res)) != NULL) {
    for (size_t i = 0; i < num_fields; i++) {
      strcat(bufresponse,row[i]);
      strcat(bufresponse,SEP_COL);
    }
    strcat(bufresponse,SEP_ROW);
  }
  strcat(bufresponse,"\0");

	mysql_free_result(res);
	mysql_close(conn);

  err->msj = "";
  err->err_code = CONN_SUCCEED;
  return err;

error_stm:
  memcpy(err->msj, mysql_error(conn), sizeof(msj));
  err->err_code = CONN_REFUSED;
  return err;
}
