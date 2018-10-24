#ifndef error_conn_h
#define error_conn_h

enum error_c {
  CONN_SUCCEED,
  CONN_REFUSED
};

typedef struct error_s{
  enum error_c err_code;
  char msj[256];
} error_connection_t;

#endif
