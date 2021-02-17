#include "tipo.h"

/* Tipos básicos*/
static Tipo _int = Tipo(TIPO_INT, "int", 4, -1, -1);
static Tipo _float = Tipo(TIPO_FLOAT, "float", 4, -1, -1);
static Tipo _double = Tipo(TIPO_DOUBLE, "double", 8, -1, -1);
static Tipo _char = Tipo(TIPO_CHAR, "char", 4, -1, -1);
static Tipo _void = Tipo(TIPO_VOID, "void", 0, -1, -1);

Tipo::Tipo(int id, string tipo, int tam, int elem, int tipoBase){
    this->id = id;
    this->tipo = tipo;
    this->tam = tam;
    this->elem = elem;
    this->tipoBase = tipoBase;
}

string Tipo::printable(){
    string s = to_string(this->id)+ "\t" + (this->tipo) + "\t";
    s += to_string(this->tam) + "\t" + to_string(this->elem) + "\t";
    s += to_string(this->tipoBase) + "\n";
    return s;
}

TablaTipos::TablaTipos(string nombreAmbiente) {
    this->nombreAmbiente = nombreAmbiente;
    vector<Tipo*> *tabla = new vector<Tipo*>;
    tabla->push_back(&_int);
    tabla->push_back(&_float);
    tabla->push_back(&_double);
    tabla->push_back(&_char);
    tabla->push_back(&_void);
    this->tabla = tabla;
}

int TablaTipos::insertar(string tipo, int elem, int tipoBase){
    /* Si ya tenemos ese tipo, regresamos el entero correspondiente*/
    for (Tipo* t : *(this->tabla)){
        if (t->tipo == tipo && t->elem == elem && t->tipoBase == tipoBase)
            return t->id;
    }
    int id = ((this->tabla)->size())+1;
    int tamano = (this->getTipo(tipoBase)->tam)*elem;
    Tipo* nuevo = new Tipo(id, tipo, tamano, elem, tipoBase);
    (this->tabla)->push_back(nuevo);
    return id;
}

Tipo* TablaTipos::getTipo(int id){
    for (Tipo *tipo : *(this->tabla)) {
        if ((tipo->id) == id)
            return tipo;
    }
    return NULL;
}

int TablaTipos::getTam(int id){
    Tipo* t = (this->getTipo(id));
    if (t!=NULL)
        return t->tam;
    else
        return -1;
}
