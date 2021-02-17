#ifndef CINTERMEDIO_
#define CINTERMEDIO_
#include <string>
#include <vector>
using namespace std;

struct Cuadrupla{
    string op;
    string arg1;
    string arg2;
    string res;

    Cuadrupla(string op, string arg1, string arg2, string res){
        this->op = op;
        this->arg1 = arg1;
        this->arg2 = arg2;
        this->res = res;
    }

    string formatoImp(void);
};

struct CodigoIntermedio {

    /* Entero que lleva la cuenta del número de variables
     * temporales creadas hasta el momento.
     */
    int temporales;
    /* Entero que lleva la cuenta del número de etiquetas
     * creadas hasta el momento.
     */
    int etiquetas;

    /* Número de índices creados*/
    int indices;

    /* lista de cuadruplas */
    vector<Cuadrupla*> *lista_codigo;


    /* Métodos */
    /* Constructor de la estructura*/
    CodigoIntermedio(void);

    /**
     * Obtiene una nueva variable temporal.
     */
    string nuevaTemporal(void);

    /**
     * Obtiene una nueva etiqueta.
     */
    string nuevaEtq(void);

    /* Obtiene un nuevo índice*/
    string nuevoIndice();

    /* Convierte un tipo mayor a otro menor con
     * un cast en el código intermedio.
     * Se asume equivalencia.
     */
    string ampliar(string dir1, int menor, int mayor);

    /* Convierte un tipo menor a otro mayor con
     * un cast en el código intermedio.
     * Se asume equivalencia.
     */
    string reducir(string dir1, int menor, int mayor);

    /* Reemplaza índices por etiquetas nuevas*/
    void reemplazarIndices(vector<string>& lista_indices, string etq);

    /**
     * Genera el código intermedio.
     */
    void genCod(string op, string arg1, string arg2, string res);

    /**
     * Genera código intermedio a partir de una cuadrupla.
     */
    void genCodCuad(Cuadrupla* cuad);

    /* Genera el archivo con código intermedio*/
    void genFile(FILE *outputfile);

};

#endif
