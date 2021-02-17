#ifndef TOKENS_H
#define TOKENS_H
/* Caracteres Especiales */
#define COMA 1001
#define PCOMA 1002
#define DOSPUNTOS 1003
#define CIZQ 1004
#define CDER 1005
#define PIZQ 1006
#define PDER 1007
#define LIZQ 1008
#define LDER 1009
/* Tipos */ 
#define INT  1010
#define FLOAT 1011
#define CHAR 1012
#define DOUBLE 1013
#define VOID 1014
/* Literales*/ 
#define ID 1015
#define NUM 1016
#define CADENA 1017
/* Operadores*/
#define ASIG 1018
#define NOT 1019
#define AND 1020
#define OR 1021
#define EQ 1022
#define NEQ 1023
#define MENOR 1024
#define MAYOR 1025
#define MENOROIGUAL 1026
#define MAYOROIGUAL 1027
#define MAS 1028
#define MENOS 1029
#define POR 1030
#define DIV 1031
#define MOD 1032
/* Palabras reservadas */
#define FUNC 1033
#define _IF 1034
#define _ELSE 1035
#define WHILE 1036
#define DO 1037
#define BREAK 1038
#define SWITCH 1039
#define CASE 1040
#define DEFAULT 1041
#define _TRUE 1042
#define _FALSE 1043
#define RETURN 1044
#define PRINT 1045
#define SCAN 1046
#define FIN 9001
/* 
 * podemos definir más o cambiar las definiciones 
 * siempre y cuando actualicemos el lexer 
 */

#include <string>
using namespace std;

/**
 * Estructura para modelar tokens (componentes léxicos)
 */
struct Token {
    int clase; /* Clase Léxica. */
    string valor; /* Valor del Lexema */
    int tipo; /* Diferencía entre tipos de lexemas en la  misma clase léxica */

    Token(int clase, string valor, int tipo){
        this->clase = clase;
        this->valor = valor;
        this->tipo = tipo;
    }
};

/**
 * Inicialización de tokens sin tipo
 */
Token *crea_token(int clase, string valor);

/**
 * Inicialización de tokens con tipo
 */
Token *crea_token_tipo(int clase, string valor, int tipo);

/**
 * Verificación de token perteneciente a una clase léxica
 */
int equals(Token *t1, int clase);

/**
 * Liberación de memoria de un token
 */
void libera(Token *t);

#endif


