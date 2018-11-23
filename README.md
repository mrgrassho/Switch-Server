# @Author Victorio Scafati 
# COMMANDOS PARA COMPILAR switch_server

gcc mysql_lib.c -o mysql_lib.o -Wall -g -c -L/usr/lib/mysql -lmysqlclient
gcc postgresql_lib.c -o postgresql_lib.o -g -c -lpq -I/usr/include/postgresql/
gcc switch_server.c -o sws postgresql_lib.o mysql_lib.o -ljansson -lpq -lpthread -I/usr/include/postgresql/
