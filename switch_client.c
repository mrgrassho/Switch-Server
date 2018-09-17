#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <jansson.h>

#define IP_SERVER "127.0.0.1"
#define PORT_SERVER "23232"

void usage() {
  printf("USO:\tswitch_client <nombre_de_bd> <query_sql>");
}

void print_json(json_t * obj) {
  json_t * st = json_object_get(obj, "status");
  if(!json_is_string(st)) {printf("ERROR!\n"); exit(0);}
  printf("STATUS %s", json_string_value(st));
  json_t * msj = json_object_get(obj, "msj");
  if(json_string_length(msj) > 0){
    printf(", %s", json_string_value((const json_t*) msj));
  }
  printf("\n");
  size_t i_index;
  json_t *i_value;
  json_t * table = json_object_get(obj, "data");
  json_t* k_value;
  json_array_foreach(table, i_index, i_value){
      for (size_t j = 0; j < json_array_size(i_value); j++) {
          k_value = json_array_get(i_value, j);
          if (!json_is_null(k_value))
            printf("%*s ", -10, json_string_value(k_value));
      }
      printf("\n");
  }
  json_decref(k_value);
  json_decref(msj);
  json_decref(st);
  json_decref(i_value);
  json_decref(table);
}

int main(int argc, char const *argv[]) {
  if (argc < 3) {
    usage();
    exit(1);
  }
  json_t * obj_query;
  json_t * obj_table;
  json_error_t err;
  char * data_send;
  char * data_recv;

  obj_query = json_pack("{s:s,s:s}", "db", argv[1], "query", argv[2]);
  data_send = json_dumps(obj_query, 0);
  printf("%s\n", data_send);
  // Estrucuturas de SOCKETS
  int sd;
  struct sockaddr_in pin;
  pin.sin_family = AF_INET;
  pin.sin_addr.s_addr = inet_addr(IP_SERVER);
  pin.sin_port = htons((uint16_t) PORT_SERVER);
  bzero(&pin.sin_zero, sizeof(pin.sin_zero));
  sd = socket(AF_INET,SOCK_STREAM,0);
  if (sd == -1){
    printf("Error al abrir el socket\n");
    exit(1);
  }
  if(connect(sd, (void *)&pin, sizeof(pin)) == -1){
    printf("Error al conectar el socket\n");
    exit(1);
  }
  // escribimos en el socket
  write(sd, data_send, strlen(data_send));
  sleep(1);
  // String deben terminar en NULL
  int l = read(sd, data_recv, strlen(data_recv));
  data_recv[l]='\0';
  //  ALTERNATIVA: FOR SOCKETS USE:
  //  obj_table = json_loadfd(sd, 0, &err);
  obj_table = json_loads(data_recv, 0, &err);
  if(!obj_table) {
    printf("ERROR al decodear el objecto: %i\n", json_error_code(&err));
    exit(1);
  }
  print_json(obj_table);

  json_decref(obj_query);
  json_decref(obj_table);
  free(data_send);
  free(data_recv);
  close(sd);
  return 0;
}