#include "codigointermedio.h"
#include "tipo.h"

string Cuadrupla::formatoImp(){
    if (this->op =="Label")
        return (this->op) + " " + (this->res) + ":";
    else if (this->op == "goto")
        return "goto " + this->res;
    else if (this->op == "if ")
        return (this->op) + (this->arg1)+ " " +(this->arg2) + " " + (this->res);
    else if (this->arg2 == "")
        return (this->op) + " " + (this->arg1) + " " + (this->res);
    else if (this->arg1 == "")
        return (this->res)+ " = " +(this->op) + (this->arg2);
    else
        return (this->res)+ " = " + (this->arg1)+ " " +(this->op) + " " + (this->arg2);
}

CodigoIntermedio::CodigoIntermedio(){
    this->temporales = 0;
    this->etiquetas = 0;
    this->indices = 0;
    this->lista_codigo = new vector<Cuadrupla*>;
}

string CodigoIntermedio::nuevaTemporal() {
    string tempActual = to_string(temporales++);
    return "_t_" + tempActual;
}

string CodigoIntermedio::nuevoIndice() {
    string tempActual = to_string(indices++);
    return "i" + tempActual;
}

string CodigoIntermedio::nuevaEtq(){
    string labelActual = to_string(temporales++);
    return "L" + labelActual;
}

string CodigoIntermedio::ampliar(string dir1, int menor, int mayor){
    if (menor == mayor)
        return dir1;
    /* El anterior es el único caso donde son equivalentes y mayor es INT*/
    string tmp = nuevaTemporal();
    if (mayor == TIPO_FLOAT && menor == TIPO_INT) {
        genCod("(float)", dir1, "", tmp);
        return tmp;
    }
    else if (mayor == TIPO_DOUBLE && menor==TIPO_INT) {
        genCod("(float)", dir1, "", tmp);
        return tmp;
    }
    else if (mayor == TIPO_DOUBLE && menor == TIPO_FLOAT ) {
        genCod("(double)", dir1, "", tmp);
        return tmp;
    }
    else {
        /* Ya no debe haber más casos de tipos compatibles que se amplian*/
        return dir1;
    }
}

string CodigoIntermedio::reducir(string dir1, int menor, int mayor){
    if (menor == mayor)
        return dir1;
    /* El anterior es el único caso donde son equivalentes y menor es DOUBLE*/
    string tmp = nuevaTemporal();
    if (menor == TIPO_INT && mayor == TIPO_FLOAT) {
        genCod("(int)", dir1, "", tmp);
        return tmp;
    }
    else if (mayor == TIPO_INT && menor==TIPO_DOUBLE) {
        genCod("(int)", dir1, "", tmp);
        return tmp;
    }
    else if (mayor == TIPO_FLOAT && menor == TIPO_DOUBLE ) {
        genCod("(float)", dir1, "", tmp);
        return tmp;
    }
    else {
        /* Ya no debe haber más casos de tipos compatibles que se reducen*/
        return dir1;
    }
}

void CodigoIntermedio::reemplazarIndices(vector<string>& lista_indices, string etq){
    string nueva = etq;
    int first = 0;
    for (string indice : lista_indices) {
        if (first != 0)
            nueva = this->nuevaEtq();
        for (Cuadrupla *cuad : *lista_codigo){
            if (cuad->res == indice) {
                (cuad->res) = nueva;
            }
        }
        first++;
    }
}

/**
 * Función para la generación de código intermedio.
 */
void CodigoIntermedio::genCod(string op, string arg1, string arg2, string res){
    Cuadrupla* c = new Cuadrupla(op, arg1, arg2, res);
    lista_codigo->push_back(c);
}

void CodigoIntermedio::genCodCuad(Cuadrupla* cuad){
    lista_codigo->push_back(cuad);
}

void CodigoIntermedio::genFile(FILE *outputfile){
    for (Cuadrupla *cuad : *lista_codigo){
        string printable = cuad->formatoImp();
        fprintf(outputfile, "%s\n", printable.c_str());
    }
}
