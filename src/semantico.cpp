#include "semantico.h"
#include "tipo.h"

bool equivalentesLista(vector<int> *lista, int tipo){
    if (lista == NULL)
        return tipo==TIPO_VOID;

    for (int t : *lista){
        if (!equivalentes(t, tipo))
            return false;
    }
    return true;
}

bool equivalentes(int tipo1, int tipo2){
    /* Los numéricos son equivalentes*/
    if (tipo1 >= TIPO_INT && tipo1 <= TIPO_DOUBLE){
        return (tipo2 >= TIPO_INT && tipo2 <= TIPO_DOUBLE);
    }
    /* Los demás sólo son equivalentes con ellos mismos*/
    else{
        return (tipo1 == tipo2);
    }
}

bool equivalentesListas(vector<int> *lista1, vector<int> *lista2){
    int s1 = lista1->size();
    int s2 = lista2->size();
    vector<int> l1 = *lista1;
    vector<int> l2 = *lista2;
    if (s1!=s2)
        return false;
    for (int i = 0; i<s1; i++){
        if (!equivalentes(l1[i], l2[i]))
            return false;
    }
    return true;
}
