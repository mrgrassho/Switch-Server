#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <jansson.h>

#include "error_conn.h"
#include "switch_server.h"
//#include "postgresql_lib.h"
//#include "mysql_lib.h"

#define EXPECT_GE(i, e, ...) \
    if (i < e) { __VA_ARGS__; }

#define EXPECT_SUCCESS(ret, fmt, ...) \
    EXPECT_GE(ret, 0, printf(fmt, ##__VA_ARGS__); goto End)

#define SOCKET(sockfd, domain, types, proto) \
    do { \
        sockfd = socket(domain, types, proto); \
        EXPECT_SUCCESS(sockfd, "[!] create socket fail. %s", strerror(errno)); \
    } while(0)

#define SETSOCKOPT(ret, sockfd, level, optname, optval) \
    do { \
        int opt = optval;\
        ret = setsockopt(sockfd, level, optname, &opt, sizeof(opt)); \
        EXPECT_SUCCESS(ret, "[!] setsockopt fail. %s", strerror(errno)); \
    } while(0)

#define BIND(ret, sockfd, addr, port) \
    do { \
        struct sockaddr_in s_addr = {}; \
        struct sockaddr sa = {}; \
        socklen_t len = 0; \
        ret = getsockname(sockfd, &sa, &len); \
        EXPECT_SUCCESS(ret, "[!] getsockopt fail. %s", strerror(errno)); \
        s_addr.sin_family = AF_INET; \
        s_addr.sin_addr.s_addr = inet_addr(addr); \
        s_addr.sin_port = htons(port); \
        ret = bind(sockfd, (struct sockaddr *) &s_addr, sizeof(s_addr)); \
        EXPECT_SUCCESS(ret, "[!] bind fail. %s", strerror(errno)); \
    } while(0)

#define LISTEN(ret, sockfd, backlog) \
    do { \
        ret = listen(sockfd, backlog); \
        EXPECT_SUCCESS(ret, "listen fail. %s", strerror(errno)); \
    } while(0)


#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

#define IP_SERVER "0.0.0.0"
#define PORT_SERVER 23232
#define META_PATH "../data/db_info.json"
#define STR_SIZE 100

void usage() {
  printf("USO:\tswitch_server [--debug]\n");
}

// DEBUG: TEST func
json_t * strtojson(char * str)
{
    json_t * array = json_array();
    json_auto_t * row = json_array();
    char * tmp;
    char x;
    tmp = malloc(STR_SIZE * sizeof(int) + 1);
    tmp[STR_SIZE] = '\0';
    while ( (x = *str++) != '\0')
    {
        if (SEP_COL[0] == x)
        {
            json_auto_t * s = json_string(tmp);
            json_array_append_new(row, s);
            tmp = "";
        }
        else if (SEP_ROW[0] == x)
        {
            json_array_append(array, row);
            json_array_clear(row);
        }
        else if (0x20 == x)
        { // IGNORAMOS ESPACIOS
            continue;
        }
        char aux[2];
        aux[0] = x;
        aux[1] = '\0';
        strcat(tmp, aux);
    }
    return array;
}

void lookup_engine(char* dbname, const char* engine)
{
    json_error_t json_err;
    json_auto_t * obj;
    FILE* fp = fopen(META_PATH, "r");
    obj = json_loadf(fp, 0, &json_err);
    const char *key;
    json_auto_t *value;
    json_object_foreach(obj, key, value) {
        const char *key_2;
        json_auto_t *value_2;
        json_object_foreach(value, key_2, value_2)
        {
            if (json_object_get(value_2, dbname) != NULL)
            {
                engine = json_string_value(value);
                break;
            }
        }
    }
}

error_connection_t process_query(char p[], char* response)
{
    json_error_t json_err;
    json_auto_t * q;
    error_connection_t err;
    q = json_loads(p, 0, &json_err);
    if(!q) goto error_pq;
    char* dbname = json_dumps(json_object_get(q, "db"), 0);
    char* qry = json_dumps(json_object_get(q, "query"), 0);
    char* engine = "";
    lookup_engine(dbname, engine);
  /* TODO; switch (engine[0])
    {
        case 'p': err = query_sw_postgresql(dbname, qry, response);
        //case 'm': err = query_sw_mysql(dbname, q, &response);
        // TODO Firebird
    } */
    return err;
    error_pq:
        printf("ERROR al decodear el objecto: %i\n", json_error_code(&json_err));
        exit(1);
}

json_t* format_response(json_t* raw_json){
    json_t* obj = json_object();
    json_object_set(obj, "status", json_string("OK"));
    json_object_set(obj, "msj", json_string(""));
    json_object_set(obj, "data", raw_json);
    return obj;
}

void * query_handler(void *p_sockfd)
{
    int ret = -1;
    char query[BUF_SIZE] = {};
    char response[BUF_SIZE] = {};
    char raw_json[BUF_SIZE] = {};
    int c_sockfd = *(int *) p_sockfd;
    #ifdef DEBUG_MODE
    printf("Query Handler creado.\nPID = %s\n", "1000");
    #endif
    // DEBUG
    json_error_t json_err;
    FILE* fp = fopen("config_test.json", "r");
    json_auto_t* obj_query = json_loadf(fp, 0, &json_err);
    // == agregar validacion de json_err
    fclose(fp);
    //
    while(1)
    {
        bzero(query, sizeof(query));
        // [+] Servidor espera codigo SQL
        ret = recv(c_sockfd, query, sizeof(query) - 1, 0);
        if (ret < 0) break;
        // TODO: [+] Servidor procesa query_sql
        //process_query(query, response);
        // [+] Convierte de string a JSON
        // [+] Se da formato a la response

        //json_auto_t * obj_query = format_response(strtojson(response));
        memcpy(raw_json, json_dumps(obj_query, 0), BUF_SIZE);
        // [+] Servidor envia respuesta
        if (send(c_sockfd, raw_json, sizeof(raw_json) - 1, 0) < 0)
            printf("ERROR al escribir en el socket\n");
    }
    if (c_sockfd < 0) close(c_sockfd);
    pthread_exit(NULL);
    // [+] Semaforo se desbloquea
}

void switch_query(const char* dbname, const char* engine, char* response)
{   /* TODO;
    switch (engine[0])
    {
        case 'p': query_sw_postgresql(dbname, ".tables", response);
        //case 'm': err = query_sw_mysql(dbname, '.tables', response);
        // TODO Firebird
    }*/
}

void query_metadata(char * response){
    json_error_t json_err;
    json_auto_t * obj;
    FILE* fp = fopen(META_PATH, "r");
    obj = json_loadf(fp, 0, &json_err);
    // == agregar validacion de json_err
    fclose(fp);
    // [+] Para cada BD de cada motor preguntar los nombres de las tablas
    const char *key;
    json_auto_t *value;
    json_object_foreach(obj, key, value)
    {
        const char *key_2;
        json_auto_t *value_2;
        json_object_foreach(obj, key_2, value_2){
            switch_query(key_2 , key, response);
        }
    }
}

void * udpate_metadata()
{
    // IMPLEMENTACION ACTUAL
    // -> Thread cantidad 60s pregunta Metadata de tablas a todas las BDs
    while (1)
    {
        // [*] Semaforo en espera
        // [+] Se realizan queries a todas las BDs preguntando por los nombres de tablas
        char response[BUF_SIZE] = {};
        query_metadata(response);
        // [+] Se castean el char* retornado a JSON
        json_auto_t * obj_query = strtojson(response);
        // [+] Se guardan los resultados en 'data/db_info.json'
        FILE* fp = fopen(META_PATH, "w+");
        json_dumpf(obj_query,fp, 0);
        fclose(fp);
        sleep(60);
        // [*] Semaforo se bloquea
    }
}

int main(int argc, char *argv[]){
    if (argc > 2)
    {
        usage();
        exit(1);
    }
    if (argc < 1)
        if ((strcmp(argv[1],"--debug") == 0) || (strcmp(argv[1],"-d") == 0)){
            #define DEBUG_MODE 3
        }
    #ifdef DEBUG_MODE
    printf("\n\t\t\t\t<< DEBUG INFO >>\n\n");
    #endif
    // [+] Servidor Actualiza Metadata de tablas
    // [+] Servidor inicializa SOCKETS
    int ret = -1, sockfd = -1, c_sockfd = -1;
    int port = PORT_SERVER;
    char addr[] = IP_SERVER;
    struct sockaddr_in c_addr = {};
    socklen_t clen = 0;
    pthread_t thr_srvr;
    pthread_t thr_updt;

    // [+] Servidor en escucha
    SOCKET(sockfd, AF_INET, SOCK_STREAM, 0);
    SETSOCKOPT(ret, sockfd, SOL_SOCKET, SO_REUSEADDR, 1);
    BIND(ret, sockfd, addr, port);
    LISTEN(ret, sockfd, 10);

    // [+] thread que actualiza el archivo de tablas
    /*ret = pthread_create(&thr_updt, NULL, udpate_metadata, NULL);
    if (ret < 0)
    {
        printf("[-] Error al iniciar el thread.\n");
        exit(1);
    }*/
    while (1)
    {
        #ifdef DEBUG_MODE
        printf("Esperando conexión...\n");
        #endif
        c_sockfd = accept(sockfd, (struct sockaddr *)&c_addr, &clen);
        if (c_sockfd < 0) continue;
        #ifdef DEBUG_MODE
        printf("Recibiendo informacion...\n");
        #endif
        // [+] Servidor crea thread que atienda al usuario
        ret = pthread_create(&thr_srvr, NULL, query_handler, (void *)&c_sockfd);
        if (ret < 0)
        {
            close(c_sockfd);
            continue;
        }
    }
  End:
    if (sockfd < 0) close(sockfd);
    return 0;
}
