#ifndef SIMBOLO_
#define SIMBOLO_

/* Diferentes tipos de símbolos*/
#define SVAR 3001
#define SFUNC 3002
#define SPARAM 3003

#include <string>
#include <vector>
#include "tokens.h"
using namespace std;

/**
 * Estructura para modelar símbolos de la Tabla de Símbolos.
 */
struct Simbolo {
    string id; /* Identificador. */
    int tipo;  /* Tipo de la variable. */
    int dir;   /* Dirección. */
    int var;   /* Tipo de símbolo (func, param, var)*/
    vector<int> *args; /* Tipos de los Argumentos */

    /* Inicializador de un símbolo */
    Simbolo(string id, int tipo, int dir, int var, vector<int> *args);

    /* Regresa una cadena para imprimir.*/
    string printable(void);
};

/**
 * Estructura wrapper para la tabla de símbolos.
 */
struct TablaSimbolos {
    string nombreAmbiente;
    vector<Simbolo*> *tabla;

    /* Constructor*/
    TablaSimbolos(string nombreAmbiente);

    /* Busca el símbolo en la lista. */
    Simbolo* busca(string id);

    void insertar(string id, int tipo, int dir, int var, vector<int> *args);
};
#endif
