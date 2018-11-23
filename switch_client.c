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
#define PORT_SERVER 23232
#define BUF_SIZE 1024

void usage() {
  printf("USO:\tswitch_client [OPCION] [IP_SERVER] [PORT]\
          \n\tIP SERVER, default 127.0.0.1\n\tPORT, default 23232\nOPCION:\n\t-h, --help\tmuestra este mensaje.\n");
}

void print_json(json_t * obj) {
  json_auto_t * st = json_object_get(obj, "status");
  if(!json_is_string(st)) {printf("ERROR!\n"); exit(0);}
  printf("STATUS %s", json_string_value(st));
  json_auto_t * msj = json_object_get(obj, "msj");
  if(json_string_length(msj) > 0){
    printf(", %s", json_string_value((const json_t*) msj));
  }
  printf("\n");
  size_t i_index;
  json_auto_t *i_value;
  if (0 != json_array_size(json_object_get(obj, "data"))){
    json_auto_t * table = json_object_get(obj, "data");
    json_auto_t* k_value;
    if (0 != json_array_size(table) && json_is_array(table)) {
      json_array_foreach(table, i_index, i_value){
          json_auto_t *j_value;
          size_t j_index;
          json_array_foreach(i_value, j_index, j_value){
            printf("%*s ", -16, json_string_value(j_value));
          }
          printf("\n");
      }
    }
  }
}

int main(int argc, char const *argv[]) {
  if (argc == 2) {
    if ((0 == strcmp(argv[1], "-h")) || (0 == strcmp(argv[1], "--help"))) {
      usage();
      exit(1);
    }
  }
  json_auto_t * obj_query;
  json_auto_t * obj_table;
  json_error_t err;
  char data_recv[BUF_SIZE] = {};


  // Estrucuturas de SOCKETS
  int sd;
  struct sockaddr_in pin;
  pin.sin_family = AF_INET;
  if (3 == argc){
      pin.sin_addr.s_addr = inet_addr(argv[1]);
      pin.sin_port = htons(atoi(argv[2]));
  } else {
      pin.sin_addr.s_addr = inet_addr(IP_SERVER);
      pin.sin_port = htons(PORT_SERVER);
  }
  printf("IP: %s, PORT: %i\n", inet_ntoa(pin.sin_addr), ntohs(pin.sin_port) );
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
  printf("Conectado al Switch Server.\n");
  printf("Sintaxis: <nombre_de_bd> <query>\n");
  char * data_send = malloc(BUF_SIZE);
  while (1) {
      // CORREGIRR
      bzero(data_send, BUF_SIZE);
      printf("%s\n", data_send);
      printf("> ");
      char sind[2][1024];
      scanf(" %s %[^\n]%*c", sind[0], sind[1]);
      obj_query = json_pack("{s:s,s:s}", "db", sind[0], "query", sind[1]);
      json_dumpb( obj_query, data_send, BUF_SIZE, 0);
      // escribimos en el socket
      write(sd, data_send, strlen(data_send));
      sleep(1);
      // String deben terminar en NULL
      bzero(data_recv, BUF_SIZE);
      int l = read(sd, data_recv, sizeof(data_recv) + 1);
      if (l == -1) {
        printf("ERROR en el read()");
        exit(EXIT_FAILURE);
      }
      //  ALTERNATIVA: FOR SOCKETS USE:
      //  obj_table = json_loadfd(sd, 0, &err);
      obj_table = json_loads(data_recv, 0, &err);
      if(!obj_table) {
        printf("[!] ERROR al decodear el objecto. ERROR MSJ: %s\n", err.text);
        exit(1);
      }
      print_json(obj_table);

  }
  free(data_send);
  close(sd);
  return 0;
}
