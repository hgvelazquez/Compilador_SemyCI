#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "parser.h"

extern FILE *yyin;
extern Token *yylex();

/* Variable para el archivo donde se escribirá
   el código intermedio. */
FILE *ciout;

/* La extensión para los archivos de salida.*/
const char *extension = ".ci";

const char *get_output_filename(const char *filename) {
    const char *dot = strrchr(filename, '.');
    int end = strlen(filename);
    if(!dot || dot == filename) { 
        char *auxname = (char*)malloc(sizeof(char) * (end));;
        strcpy(auxname, filename);
        char *newname = strcat(auxname, extension);
        return newname;
    } else {
        int x = strlen(filename)-strlen(dot);
        char *noext = (char*)malloc(sizeof(char) * (x));
        strncpy(noext, filename, x);
        char* newname = strcat(noext, extension);
        return newname;
    }
}


int main(int argc, char **argv) {
  if(argc < 2) {
    puts("Falta archivo de entrada.");
    exit(1); 
  }
  
  printf("Archivo de entrada: ");
  puts(argv[1]);
  FILE *fin = fopen(argv[1], "r");
  yyin = fin;
  
  FILE *fout = fopen(get_output_filename(argv[1]), "w");
  ciout = fout;
  
  puts("Comenzando análisis...");
  parse();
  fclose(fout);
}
