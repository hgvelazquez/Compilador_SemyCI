#ifndef TIPO_
#define TIPO_
#define TIPO_INT 1
#define TIPO_FLOAT 2
#define TIPO_DOUBLE 3
#define TIPO_CHAR 4
#define TIPO_VOID 5
#define TIPO_CADENA 3000

#include <string>
#include <vector>
using namespace std;

/**
 * Estructura para modelar entradas de la Tabla de Tipos.
 */
struct Tipo {
    int id; /* identificador del tipo */
    string tipo;
    int tam; /* Tamaño*/
    int elem; /* Numero de elementos en los compuestos. */
    int tipoBase; /* Tipo base de los compuestos*/

    /* Constructor de Tipos.*/
    Tipo(int id, string tipo, int tam, int elem, int tipoBase);

    /* Regresa una cadena para imprimir*/
    string printable();
};

/**
 * Estructura Wrapper para la Tabla de tipos.
 */
struct TablaTipos {
    string nombreAmbiente;
    vector<Tipo*> *tabla;

    /* Constructor de la tabla de tipos.
     * Mete los tipos básicos a la tabla desde el inicio.
     */
    TablaTipos(string nombreAmbiente);


    /* Inserta un nuevo tipo con esos datos a la tabla de tipos*/
    int insertar(string tipo, int elem, int tipoBase);

    /* Busca el tipo con ese id en la tabla y lo regresa */
    Tipo* getTipo(int id);

    /* Regresa el tamaño del tipo con el id recibido*/
    int getTam(int id);
};

#endif
