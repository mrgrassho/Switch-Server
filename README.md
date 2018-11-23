###### @Author Victorio Scafati

# Switch Server Architecture

<dl>
<img src="https://github.com/mrgrassho/Switch-Server/blob/master/SWS_Diagram.png" alt="SWS_Diagram" class="center">
</dl>

#### Commands to compile switch_server:
```
gcc mysql_lib.c -o mysql_lib.o -Wall -g -c -L/usr/lib/mysql -lmysqlclient
gcc postgresql_lib.c -o postgresql_lib.o -g -c -lpq -I/usr/include/postgresql/
gcc switch_server.c -o sws postgresql_lib.o mysql_lib.o -ljansson -lpq -lpthread -I/usr/include/postgresql/
```

#### Dependencies:

	-	Jansson (https://github.com/akheron/jansson/blob/2.11/doc/index.rst)
	-	MySQL 5.7  - C Connector (https://dev.mysql.com/doc/)
	-	PostgreSQL 9.0 - C Connector (https://www.postgresql.org/download/)
