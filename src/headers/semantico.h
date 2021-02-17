#ifndef SEMATICO_
#define SEMANTICO_
#include "tipo.h"
#include <vector>
#include <string>
using namespace std;

bool equivalentes(int tipo1, int tipo2);
bool equivalentesLista(vector<int> *lista, int tipo);
bool equivalentesListas(vector<int> *lista1, vector<int> *lista2);

#endif
