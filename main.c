#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void parse_str(char* tok2){

  char *tok1 = strtok(tok2, ",");
  while (tok1) {
    puts(tok1);
    tok1 = strtok(NULL, ",");
  }
}

char * ddd() {return "dsdsdsds";}

int main(int argc, char const *argv[]) {
  char * str = "Kuka Trap";
  char * ptr;
  char r = ddd()[0];
  printf("%c\n", r);
  ptr = str;
  printf("%s\n", ptr);
  ++ptr;
  char x = *str++;
  printf("%c\n", *(++ptr));
  printf("%c\n", x);
  char * bd[2] = {"FACTURA", "PEDIDO"};
   char input[] = "FACTURA,CANT,PRECIO|100,323,100.33";
   printf("Parsing the input string '%s'\n", input);
   char *tok2 = strtok(input, "|");
   while(tok2) {
      puts(tok2);

      char aux[20];
      memcpy(aux, tok2, sizeof(aux));
      parse_str(aux);
      tok2 = strtok(NULL, "|");
   }
  return 0;
}
