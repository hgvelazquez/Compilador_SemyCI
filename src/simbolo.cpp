#include "simbolo.h"

Simbolo::Simbolo(string id, int tipo, int dir, int var, vector<int> *args) {
    this->id = id;
    this->tipo=tipo;
    this->dir = dir;
    this->var=var;
    this->args = args;
}

string Simbolo::printable(){
    string s = (this->id)+ "\t" + to_string(this->tipo) + "\t";
    s += to_string(this->dir) + "\t";
    
    /* La parte de var */
    if (this->var == SVAR)
        s += "var\t";
    else if (this->var == SFUNC)
        s+= "func\t";
    else if (this->var == SPARAM)
        s+= "param\t";
    else
        s+="\t";
    
    /* Los argumentos*/        
    if (args != NULL){
        for (int i : *args){
            s += to_string(i) + ",";
        }
    }else {
        s+="---";
    }
    s+="\n";
    return s;
}

TablaSimbolos::TablaSimbolos(string ambiente) {
    this->nombreAmbiente = ambiente;
    vector<Simbolo*> *table = new vector<Simbolo*>;
    this->tabla = table;
}

Simbolo* TablaSimbolos::busca(string id) {
    for (Simbolo *simbolo : *(this->tabla)) {
        if ((simbolo->id) == id)
            return simbolo;
    }
    return NULL;
}

void TablaSimbolos::insertar(string id, int tipo, int dir, int var, vector<int> *args)
{
    Simbolo* nuevo = new Simbolo(id, tipo, dir, var, args);
    (this->tabla)->push_back(nuevo);
}
