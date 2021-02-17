#include "parser.h"
#include <stdio.h>
using namespace std;

/* Variables externas */
extern Token *yylex(); /* Función para el parsing. */
extern int yylineno;   /* Número de línea leída */
extern FILE *ciout;    /* Archivo para escribir al código intermedio*/


/* Token actual en la lectura */
Token *tokenActual;
/* Dirección disponible en la TS */
int dir;
/* Pila de Tablas de símbolos */
stack<TablaSimbolos*> pilaTS;
/* Fondo de la pila tabla de símbolos (ambiente global)*/
TablaSimbolos* tablaSimbolosGlobal;
/* Pila de Tablas de tipos */
stack<TablaTipos*> pilaTT;
/* Fondo de la tabla de tipos (ambiente global). */
TablaTipos* tablaTiposGlobal;
/* Pila de direcciones. */
stack<int> pilaDir;
/* Lista de Retorno actual */
vector<int> *listaRetorno;
/* Generador de código intermedio */
CodigoIntermedio ci;


Token* eat(int clase) {
    if(equals(tokenActual, clase)) {
        Token* tokenAnterior = tokenActual;
        tokenActual = yylex();
        return tokenAnterior;
    } else {
        error("Error de sintaxis se esperaba: " + to_string(clase));
        // Dummy return, error termina el programa. Es para evitar warnings.
        return tokenActual;
    }
}

void parse() {
    /* Inicializamos el tokenActual*/
    tokenActual = yylex();
    /* Iniciamos el análisis con la producción inicial*/
    programa();
    /* Verificamos que el archivo se haya leído en su totalidad.*/
    if(equals(tokenActual, FIN)) {
        printf("Fin de análisis sintáctico y semántico. La cadena fue aceptada.\n");
        /* Generamos el archivo .ci*/
        ci.genFile(ciout);
        return;
    } else {
        error("Error de sintaxis. Se esperaba fin de archivo.");
    }
}

void programa() {
    /* Iniciamos con pilaTS.push(nuevaTablaTS()) y
       pilaTT.push(nuevaTablaTT()), pero debemos mantener
       el acceso a ellas pues <stack> no nos lo brinda.
     */
    TablaTipos ttg = TablaTipos("GLOBAL");
    TablaSimbolos tsg = TablaSimbolos("GLOBAL");
    tablaTiposGlobal = &ttg;
    tablaSimbolosGlobal = &tsg;
    pilaTT.push(tablaTiposGlobal);
    pilaTS.push(tablaSimbolosGlobal);
    /* Hacemos dir = 0 */
    dir = 0;
    /* Inicia el análisis sintáctico. */
    declaraciones();
    funciones();
    /* Terminamos imprimiendo las tablas globales*/
    printTS(tablaSimbolosGlobal);
    printTT(tablaTiposGlobal);
}

void declaraciones(){
    int t_tipo = tipo();
    /* Hacemos un if para ver si encontramos un tipo. */
    if (t_tipo != -1){
        /* Le pasamos el atributo heredado a lista_var*/
        lista_var(t_tipo);
        eat(PCOMA);
        declaraciones();
    }
    /* Si no encontramos la producción era
     * declaraciones -> epsilon, por lo que
     * no marcamos un error. Ese se maneja al descender.
     */
}

int tipo(){
    int b = basico();
    /* Vemos si encontramos un tipo. */
    if (b != -1) {
        int c = compuesto(b);
        return c;
    }
    return -1;
    /* No se leyó un tipo. Esto puede ser por la epsilon-
     * producción de declaraciones, por lo que el error
     * se deja para manejar al descender.
     */
}

/* Regresa -1 si no lee un tipo básico, para manejar el caso
 * de las producciones epsilon en nivel superior.
 */
int basico(){
    if (equals(tokenActual, INT)) {
        eat(INT);
        return TIPO_INT;
    }
    else if(equals(tokenActual, FLOAT)) {
        eat(FLOAT);
        return TIPO_FLOAT;
    }
    else if(equals(tokenActual, CHAR)) {
        eat(CHAR);
        return TIPO_CHAR;
    }
    else if(equals(tokenActual, DOUBLE)) {
        eat(DOUBLE);
        return TIPO_DOUBLE;
    }
    else if(equals(tokenActual, VOID)) {
        eat(VOID);
        return TIPO_VOID;
    }
    else
        return -1;
}

int compuesto(int base_tipo){
    /* Manejamos la epsilon producción gracias a que la otra inicia
     * con el terminal "["
     */
    if (equals(tokenActual, CIZQ)) {
        eat(CIZQ);
        /* Obtener valor y verificar que sea entero*/
        Token* numero = eat(NUM);
        int elem = stoi(numero->valor);
        /* Revisamos que sea entero.*/
        if (numero->tipo != TIPO_INT){
            error("Sólo se puede indexar con enteros");
        }
        eat(CDER);
        /* Insertamos a la lista el tipo con la info. del sintetizado*/
        int compuesto1_tipo = compuesto(base_tipo);
        TablaTipos tablaActual = *(pilaTT.top());
        int compuesto_tipo = tablaActual.insertar("array", elem, compuesto1_tipo);
        /* compuesto.tipo = PilaTT.top().insertar...*/
        return compuesto_tipo;
    } else {
        /* es la epsilon producción.*/
        return base_tipo;
    }
}

void lista_var(int t_tipo){
    Token* id_l = eat(ID);
    /* Acciones semánticas. */
    TablaSimbolos tsActual = *(pilaTS.top());
    TablaTipos ttActual = *(pilaTT.top());
    /* Si no está, la insertamos a la tabla de símbolos. */
    if (!tsActual.busca(id_l->valor)) {
        tsActual.insertar(id_l->valor, t_tipo, dir, SVAR, NULL);
		dir = dir + ttActual.getTam(t_tipo);
    } else {
        error("El id '" + (id_l->valor) + "' ya está declarado");
    }
    /* Fin de acciones semánticas */
    lista_varP(t_tipo);
}

void lista_varP(int lv_tipo){
    /* Caso de la producción que no es epsilon. */
    if (equals(tokenActual, COMA)){
        eat(COMA);
        Token* id_l = eat(ID);
        /* Inicio de las acciones semánticas */
        TablaSimbolos tsActual = *(pilaTS.top());
        TablaTipos ttActual = *(pilaTT.top());
        /* Si no está, la insertamos en la tabla de símbolos. */
        if (!tsActual.busca(id_l->valor)) {
            tsActual.insertar(id_l->valor, lv_tipo, dir, SVAR, NULL);
    		dir = dir + ttActual.getTam(lv_tipo);
        } else {
            error("El id '" + id_l->valor + "' ya está declarado");
        }
        /* Fin de acciones semánticas */
        lista_varP(lv_tipo);
    }
    /* El else es la producción epsilon, pero en este caso no hacemos nada.*/
}


void funciones(){
    /* Creamos una lista de Retorno vacía.*/
    vector<int> lr;
    listaRetorno = &lr;
    if(equals(tokenActual, FUNC)) {
        eat(FUNC);
        int t_tipo = tipo();
        if (t_tipo == -1){
            error("No se especificó un tipo válido de retorno para la función");
            return;
        }

        Tipo* ret_tipo = (pilaTT.top())->getTipo(t_tipo);
        Token* id_f = eat(ID);

        if ((pilaTS.top())->busca(id_f->valor)) {
            error("El id '" + id_f->valor + "' ya fue declarado");
        }
        /* Resto de acciones semánticas. */
        ci.genCod("Label", "", "", (id_f->valor));
        TablaSimbolos nuevaTablaSimbolos = TablaSimbolos(id_f->valor);
        TablaTipos nuevaTablaTipos = TablaTipos(id_f->valor);

        pilaTS.push(&nuevaTablaSimbolos);
		pilaTT.push(&nuevaTablaTipos);
        pilaDir.push(dir);
		dir = 0;

        eat(PIZQ);
        vector<int> *lista_argumentos = argumentos();
        eat(PDER);

        string bloque_sig = ci.nuevaEtq();
        bloque(bloque_sig);
        if (equivalentesLista(listaRetorno, t_tipo)) {
            (pilaTS.top())->insertar(id_f->valor, t_tipo, -1, SFUNC, lista_argumentos);
	        ci.genCod("Label", "", "", (bloque_sig));
        } else {
            string err_msg = "Los tipos de retorno no coinciden con los tipos de ";
            err_msg += "retorno declarados de la función '" + id_f->valor + "'";
            error(err_msg);
        }
        /* Imprimimos antes de hacer el pop()*/
        printTS(pilaTS.top());
        printTT(pilaTT.top());

		pilaTS.pop();
		pilaTT.pop();
		dir = pilaDir.top();
        pilaDir.pop();
        /* Modificación, se añaden al global también*/
		tablaSimbolosGlobal->insertar(id_f->valor, t_tipo, -1, SFUNC, lista_argumentos);
        /* Fin de acciones semánticas.*/
        funciones();
    }
    /* Si no inicia con func, entonces es la
     * otra producción que es la cadena vacía.
     * no hacemos nada.
     */
}

vector<int>* argumentos(){
    /* Si argumentos -> epsilon se pasa
     * el siguiente símbolo es un paréntesis derecho
     * y sólo regresamos una lista vacía.
     */
    if (!equals(tokenActual, PDER))
        return lista_args();
    else /* Produción epsilon*/
        return new vector<int>;
}

vector<int>* lista_args(){
    int t_tipo = tipo();

    if (t_tipo == -1) {
        error("Tipo inválido en los argumentos");
    }

    Token *id_t = eat(ID);
    /* Acciones semánticas */
    if ((pilaTS.top())->busca(id_t->valor)) {
        error("El id '" + id_t->valor + "'ya está declarado");
    }
    pilaTS.top()->insertar(id_t->valor, t_tipo, dir, SPARAM, NULL);
    dir = dir + pilaTT.top()->getTam(t_tipo);
    vector<int> *listaH = new vector<int>;
    listaH -> push_back(t_tipo);
    return lista_argsP(listaH);
    /* Fin de acciones semánticas*/
}

vector<int>* lista_argsP(vector<int>* listaH){
  if (equals(tokenActual, COMA)) {
      eat(COMA);
      int t_tipo = tipo();
      if (t_tipo == -1) {
          error("Tipo inválido en los parámetros");
      }
      Token *id_t = eat(ID);

      /* Acciones semánticas */
      if ((pilaTS.top())->busca(id_t->valor)) {
          error("El id ya está declarado\n");
      }

      pilaTS.top()->insertar(id_t->valor, t_tipo, dir, SPARAM, NULL);
      dir = dir + pilaTT.top()->getTam(t_tipo);
      listaH -> push_back(t_tipo);
      return lista_argsP(listaH);
      /* Fin de acciones semánticas*/
  } /* Si no, se produjo la cadena vacía.*/
  else {
      return listaH;
  }
}

void bloque(string sig){
    eat(LIZQ);
    declaraciones();
    instrucciones(sig);
    eat(LDER);
}

void instrucciones(string sig){
    string sentencia_sig = ci.nuevaEtq();
    sentencia(sentencia_sig);
    ci.genCod("Label", "", "", (sentencia_sig));
    instruccionesP(sentencia_sig);
}

void instruccionesP(string sig){
    if (equals(tokenActual, _IF) || equals(tokenActual, WHILE) ||
        equals(tokenActual, DO) || equals(tokenActual, BREAK) ||
        equals(tokenActual, LIZQ) || equals(tokenActual, RETURN) ||
        equals(tokenActual, SWITCH) || equals(tokenActual, PRINT) ||
        equals(tokenActual, SCAN) || equals(tokenActual, ID)) {
    /* Tenemos que revisar FIRST(sentencia) para saber si íbamos
     * a leer una sentencia o no, pues sentencia no es anulable.
     * por lo que si la producción era
     * instrucciones1 -> sentencia instrucciones1
     * necesariamente leeremos FIRST(sentencia).
     */
        string sentencia_sig = ci.nuevaEtq();
        sentencia(sentencia_sig);
        ci.genCod("Label", "", "", (sentencia_sig));
        instruccionesP(sentencia_sig);
    }
    /* Si no leímos una sentencia teníamos instruccionesP -> epsilon,
     * pues sentencia no es anulable.
     */
}

void sentencia(string sig){
    /* Producción sentencia -> if (bool) sentencia sentenciaP*/
    if (equals(tokenActual, _IF)){
        eat(_IF);
        eat(PIZQ);
        string bool_vddr =ci.nuevaEtq();
        string bool_fls = ci.nuevoIndice();
        bool_(bool_vddr, bool_fls);
        eat(PDER);
        /* sentencia1.sig = sentencia.sig*/
        ci.genCod("Label", "", "", bool_vddr);
        sentencia(sig);
        vector<string> lista_indices = *(new vector<string>);
		lista_indices.push_back(bool_fls);
        ci.genCod("Label", "", "", bool_fls);
        sentenciaP(sig, lista_indices);
    /* Producción sentencia -> while (bool) sentencia*/
    } else if (equals(tokenActual, WHILE)){
        eat(WHILE);
        eat(PIZQ);
        string sentencia1_sig = ci.nuevaEtq();
	    string bool_vddr =ci.nuevaEtq();
		string bool_fls = sig;
        /* Maybe*/
        ci.genCod("Label", "", "", (sentencia1_sig));
        bool_(bool_vddr, bool_fls);
        eat(PDER);
        ci.genCod("Label", "", "", (bool_vddr));
        sentencia(sentencia1_sig);
        ci.genCod("goto", "", "", sentencia1_sig);
    /* Producción sentencia -> do sentencia while (bool)*/
    } else if (equals(tokenActual, DO)){
        eat(DO);
        string bool_vddr =ci.nuevaEtq();
        string bool_fls = sig;
	    string sentencia1_sig =ci.nuevaEtq();

        ci.genCod("Label", "", "", (bool_vddr));
        ci.genCod("Label", "", "", (sentencia1_sig));
        sentencia(sentencia1_sig);
        eat(WHILE);
        eat(PIZQ);
        bool_(bool_vddr, bool_fls);
        eat(PDER);

    /* Producción sentencia -> break; */
    } else if (equals(tokenActual, BREAK)){
        eat(BREAK);
        ci.genCod("goto", "", "", sig);
        eat(PCOMA);
    /* Producción sentencia -> bloque)*/
    } else if (equals(tokenActual, LIZQ)){
        /* El bloque necesariamente inicia con {*/
        bloque(sig);
    /* Producción sentencia -> return sentenciaPP*/
    } else if (equals(tokenActual, RETURN)){
        eat(RETURN);
        sentenciaPP();
    /* Producción sentencia -> switch (bool) {casos}*/
    } else if (equals(tokenActual, SWITCH)){
        eat(SWITCH);
        eat(PIZQ);

        pair<int, string> cbool = bool_("", "");
        string casos_etqprueba =ci.nuevaEtq();
		ci.genCod("goto", "", "", casos_etqprueba);
		string casos_sig = sig;
		string casos_id = cbool.second;

        eat(PDER);
        eat(LIZQ);

        vector<Cuadrupla*> casos_prueba;
        casos_prueba = casos(casos_id, sig, casos_prueba);
        /* Para salir del switch*/
        string finswitch = ci.nuevaEtq();
        ci.genCod("goto", "", "", finswitch);
        ci.genCod("Label", "", "", casos_etqprueba);
        for(Cuadrupla* cuad : casos_prueba){
            ci.genCodCuad(cuad);
        }


        eat(LDER);
        ci.genCod("Label", "", "", finswitch);
    /* Producción sentencia -> print exp;*/
    } else if (equals(tokenActual, PRINT)){
        eat(PRINT);
        pair<int, string> cexp = exp_();
        ci.genCod("print", cexp.second, "", "");
        eat(PCOMA);
    /* Producción sentencia -> scan parte_izquierda*/
    } else if (equals(tokenActual, SCAN)){
        eat(SCAN);
        pair<int, string> cpizq = parte_izquierda();
        ci.genCod("scan", cpizq.second, "", "");
    /* Producción sentencia -> parte_izquierda = bool;*/
    } else if (equals(tokenActual, ID)){
        /* FIRST(parte_izquierda) = {id} */
        pair<int, string> cpizq = parte_izquierda();
        eat(ASIG);
        pair<int, string> cbool = bool_("", "");
        eat(PCOMA);
        if (equivalentes(cpizq.first, cbool.first)){
            string d1 = ci.reducir(cbool.second, cbool.first, cpizq.first);
            /* Lo agregamos al segundo param para que imprima con =*/
            ci.genCod("", "", d1, cpizq.second);
        }else {
            error("Tipos incompatibles en operador '=' ");
        }
    } else {
        /* No se cumplió ninguno de los casos. */
        error("Error de sintaxis. No hay sentencia");
    }
}

void sentenciaP(string sig, vector<string>& lista_indices){
    if (equals(tokenActual, _ELSE)){
        eat(_ELSE);
        string sentencia_sig = sig;
        sentencia(sig);
        ci.reemplazarIndices(lista_indices, ci.nuevaEtq());
    }
    /* sentenciaP-> eps*/
    ci.reemplazarIndices(lista_indices, ci.nuevaEtq());
}

void sentenciaPP(){
    if (equals(tokenActual, PCOMA)){
        eat(PCOMA);
        listaRetorno->push_back(TIPO_VOID);
        ci.genCod("return",  "", "", "");
    }else {
        pair<int, string> pexp = exp_();
        listaRetorno->push_back(pexp.first);
        ci.genCod("return",  pexp.second, "", "");
        eat(PCOMA);
    }
}

vector<Cuadrupla*> casos(string id, string sig, vector<Cuadrupla*>& prueba){
    if (equals(tokenActual, CASE)) {
        /* caso sólo puede iniciar con case */
        /*caso.sig = casos.sig*/
        Cuadrupla* caso_prueba = caso(id, sig);

        /* casos.prueba = caso.prueba || casos1.prueba
         * esto lo logramos con una lista.
         */
         prueba.push_back(caso_prueba);
        /* casos1.sig = casos.sig*/

         return casos(id, sig, prueba);

    } else if (equals(tokenActual, DEFAULT)){
        /* predeterminado sólo puede iniciar con default*/
        /* predeterminado.sig = casos.sig*/
        Cuadrupla* pred = predeterminado(sig);
        /* casos.prueba = predeterminado.prueba*/
        prueba.push_back(pred);
        return prueba;
    }
    /* Si no ocurre ninguna de las dos, queda la producción
     * de la cadena vacía, en ese caso no hacemos nada.
     */
     /* Equivalente a ""*/
     return prueba;
}

Cuadrupla* caso(string id, string sig){
    string inicio = ci.nuevaEtq();

    eat(CASE);
    Token* nume = eat(NUM);
    string num_val = nume->valor;
    /*caso.prueba =*/
	ci.genCod("Label", "", "", inicio);

    eat(DOSPUNTOS);
    instrucciones(sig);

    return new Cuadrupla("if ", id + "==" + num_val, "goto", inicio);
}

Cuadrupla* predeterminado(string sig){
    string inicio = ci.nuevaEtq();
    /*predeterminado.prueba = */
    Cuadrupla* cu = new Cuadrupla("goto", "", "", inicio);
	ci.genCod("Label", "", "", inicio);

    eat(DEFAULT);
    eat(DOSPUNTOS);
    instrucciones(sig);
    return cu;
}

pair<int, string> parte_izquierda(){
    Token *id_t = eat(ID);
    /* parte_izquierdaP.base = id.lexval*/
    pair<int, string> pip = parte_izquierdaP(id_t->valor);
    return pip;
}

pair<int, string> parte_izquierdaP(string base){
    /* FIRST(localizacion) = {"["}*/
    if (equals(tokenActual, CIZQ)){
        pair<int, string> pl = localizacion(base);
        return pl;
    } else {
        /* Producción parteIzquierdaP -> epsilon*/
        Simbolo* buscaBase = (pilaTS.top())->busca(base);
        if (buscaBase != NULL) {
            string pip_dir = base;
            int pip_tipo = buscaBase->tipo;
            return make_pair(pip_tipo, pip_dir);
        } else {
            error("El id no está declarado");
            return make_pair(0, "");//evita warnings.
        }
    }
}

pair<int, string> bool_(string vddr, string fls){
    if (vddr == "") {
        vddr = ci.nuevaEtq();
        fls = ci.nuevaEtq();
    }
    string comb_vddr = vddr;
    string comb_fls = ci.nuevoIndice();

    pair<int, string> pcomb = comb(comb_vddr, comb_fls);
    int tipoH = pcomb.first;
    vector<string> bp_lista_indices = *(new vector<string>);
    bp_lista_indices.push_back(comb_fls);


    pair<int, string> pboolP = boolP(tipoH, vddr, fls, bp_lista_indices, true, comb_fls);
    int tipoS = pboolP.first;

    return make_pair(tipoS, pcomb.second);
}

pair<int, string>  boolP(int tipoH, string vddr, string fls, vector<string>& lista_indices, bool ft, string comb_fls){
    if (equals(tokenActual, OR)){
        eat(OR);
        if (ft){
            ci.genCod("Label", "", "", (comb_fls));
        }
        string comb_vddr = vddr;
        string comb_fls = ci.nuevoIndice();
        lista_indices.push_back(comb_fls);

        pair<int, string> pcomb = comb(comb_vddr, comb_fls);
        int comb_tipo = pcomb.first;
        if (! equivalentes(tipoH, comb_tipo)){
            error("Tipos Incompatibles en operador '||'");
        }

        ci.genCod("Label", "", "", comb_fls);
        pair<int, string> pbp = boolP(comb_tipo, vddr, fls, lista_indices, false, "");
        return make_pair(TIPO_INT, pcomb.second);
    } else {
    /* Si no, es la producción de epsilon*/
        ci.reemplazarIndices(lista_indices, fls);
        int tipoS = tipoH;
        return make_pair(tipoS, "");
    }
}

pair<int, string> comb(string vddr, string fls){
    string ig_vddr = ci.nuevoIndice();
    string ig_fls = fls;

    pair<int, string> pigu = igualdad(ig_vddr, ig_fls);
    int tipoH = pigu.first;
    vector<string> lista_indices = *(new vector<string>);
    lista_indices.push_back(ig_vddr);


    pair<int, string> pcomb = combP(tipoH, vddr, fls, lista_indices, true);
    return make_pair(pcomb.first, pigu.second);
}

pair<int, string>  combP(int tipoH, string vddr, string fls,  vector<string>& lista_indices, bool ft){
    if (equals(tokenActual, AND)){
        eat(AND);
        if (ft) {
            ci.genCod("Label", "", "", (vddr));
        }
        string igu_vddr = ci.nuevoIndice();
        string igu_fls = fls;
        lista_indices.push_back(igu_vddr);
        pair<int, string> pigu = igualdad(igu_vddr, igu_fls);
        /* Revisamos equivalencia de tipos*/
        if (!equivalentes(tipoH, pigu.first)){
            error("Tipos Incompatibles en operador '&&'");
        }
        int combp_tipoH = pigu.first;

        ci.genCod("Label", "", "", igu_vddr);

        pair<int, string> pcombp = combP(combp_tipoH, vddr, fls, lista_indices, false);

        return make_pair(TIPO_INT, pigu.second);
    }
    /* Si no, es la producción de epsilon*/
    ci.reemplazarIndices(lista_indices, vddr);
    int tipoS = tipoH;
    return make_pair(tipoS, "");
}

pair<int, string> igualdad(string vddr, string fls){

    pair<int, string> prel = rel(vddr, fls);
    pair<int, string> pigup = igualdadP(prel.first, prel.second, vddr, fls);
    return pigup;
    /* tipo y direcciones sintetizados de igualdadP*/
}

pair<int, string> igualdadP(int tipoH, string dirH, string vddr, string fls) {
    if (equals(tokenActual, EQ)){
        eat(EQ);
        pair<int, string> prel = rel(vddr, fls);
        if (!equivalentes(tipoH, prel.first)){
            error("Tipos Incompatibles en operador '=='");
        }
        string dirS = ci.nuevaTemporal();
        /* Los tipos equivalentes están representados por números ascendentes.
         * La función máximo equivale a sacar el máximo aritmético*/
        int tTemp = (tipoH > prel.first)? tipoH : prel.first;
        string d1 = ci.ampliar(dirH, tipoH, tTemp);
        string d2 = ci.ampliar(prel.second, prel.first, tTemp);

        ci.genCod("==", d1, d2, dirS);
        ci.genCod("if ", dirS, "goto ", vddr);
        ci.genCod("goto", "", "", fls);

        pair<int, string> pigup = igualdadP(TIPO_INT, dirS, vddr, fls);

        return make_pair(pigup.first, dirS);
    } else if (equals(tokenActual, NEQ)){
        eat(NEQ);
        pair<int, string> prel = rel(vddr, fls);
        if (!equivalentes(tipoH, prel.first)){
            error("Tipos Incompatibles en operador '!='");
        }
        string dirS = ci.nuevaTemporal();
        int tTemp = (tipoH > prel.first)? tipoH : prel.first;
        string d1 = ci.ampliar(dirH, tipoH, tTemp);
        string d2 = ci.ampliar(prel.second, prel.first, tTemp);

        ci.genCod("!=", d1, d2, dirS);
        ci.genCod("if ", dirS, "goto ", vddr);
        ci.genCod("goto", "", "", fls);

        pair<int, string> pigup = igualdadP(TIPO_INT, dirS, vddr, fls);

        return make_pair(pigup.first, dirS);
    } else {
    /* Si no, es la producción de epsilon*/
        return make_pair(tipoH, dirH);
    }
}

pair<int, string>  rel(string vddr, string fls){
    pair<int, string> pexp = exp_();

    pair<int, string> prelP = relP(pexp.first, pexp.second, vddr, fls);
    /* Regresamos los sintetizados.*/
    return prelP;
}

pair<int, string>  relP(int tipoH, string dirH, string vddr, string fls){
    pair<int, string> pexp;
    string relacion = "";
    if (equals(tokenActual, MENOR)){
        eat(MENOR);
        pexp = exp_();
        relacion = "<";
    } else if (equals(tokenActual, MENOROIGUAL)){
        eat(MENOROIGUAL);
        pexp = exp_();
        relacion = "<=";
    } else if (equals(tokenActual, MAYOROIGUAL)){
        eat(MAYOROIGUAL);
        pexp = exp_();
        relacion = ">=";
    } else if (equals(tokenActual, MAYOR)){
        eat(MAYOR);
        pexp = exp_();
        relacion = ">";
    }  else {
        /* Si no, es la producción era rel -> epsilon y ya terminamos.*/
        /* Devolvemos los sintetizados. */
        return make_pair(tipoH, dirH);
    }
    /* Si llega aquí es que pasó por alguno de los if, es
     * decir, la producción no era vacía.*/
    if (equivalentes(tipoH, pexp.first)) {
        string dirS = ci.nuevaTemporal();
        /* Obtener el máximo es equivalente al máximo aritmético*/
        int tTemp = (tipoH >= pexp.first)? tipoH: pexp.first;
        string d1 = ci.ampliar(dirH, tipoH, tTemp);
        string d2 = ci.ampliar(pexp.second, pexp.first, tTemp);

        /*AGREGADO POST*/
        ci.genCod(relacion, d1, d2, dirS);
        ci.genCod("if ", dirS, "goto ", vddr);
        ci.genCod("goto", "", "", fls);

        return make_pair(TIPO_INT, dirS);
    } else {
        error("Tipos Incompatibles en operador '" + relacion + "'");
        return make_pair(-1, ""); //dummy no se alcanza.
    }
}

pair<int, string> exp_(){
    pair<int, string> pterm = term();
    pair<int, string> pexp = expP(pterm.first, pterm.second);
    /* Regresamos los sintetizados*/
    return pexp;
}

pair<int, string> expP(int tipoH, string dirH){
    pair<int, string> pterm;
    string operador = "";
    if (equals(tokenActual, MAS)){
        eat(MAS);
        pterm = term();
        operador = "+";
    } else if (equals(tokenActual, MENOS)){
        eat(MENOS);
        pterm = term();
        operador = "-";
    } else {
        /* Si no, es la producción era expP -> epsilon.
         * Regresamos los sintetizados. */
        return make_pair(tipoH, dirH);
    }
    /* Si llegamos aquí es porque la producción no era epsilon.*/
    int term_tipo = pterm.first;
    if(!equivalentes(tipoH, term_tipo)) {
        error("Tipos Incompatibles en operador '" + operador + "'");
    }
    int tipoH1 = (tipoH>=term_tipo)? tipoH : term_tipo;
    string dirH1 = ci.nuevaTemporal();

    string d1 = ci.ampliar(dirH, tipoH, tipoH1);
  	string d2 = ci.ampliar(pterm.second, pterm.first, tipoH1);
    ci.genCod(operador, d1, d2, dirH1);

    pair<int, string> pexp1 = expP(tipoH1, dirH1);

    /* Regresamos los sintetizados.*/
    return pexp1;

}

pair<int, string>  term(){
    pair<int, string> puna = unario();
    /* Pasamos el tipo y dir de unario*/
    pair<int, string> ptp = termP(puna.first, puna.second);
    /* Regresamos los sintetizados.*/
    return ptp;
}

pair<int, string>  termP(int tipoH, string dirH){
    /* * y / Son idénticos, los manejamos en el mismo IF*/
    if (equals(tokenActual, POR) || equals(tokenActual, DIV)){
        /* Manejamos el caso correspondiente*/
        string oper = "";
        if (equals(tokenActual, POR)){
            eat(POR);
            oper = "*";
        } else { /* DIV*/
            eat(DIV);
            oper = "/";
        }
        /* Inicio de esquema de traducción*/
        pair<int, string> puna = unario();
        int un_tipo = puna.first;
        if(!equivalentes(tipoH, un_tipo)) {
            error("Tipos Incompatibles en operador '" + oper + "'");
        }
        int tipoH1 = (tipoH>=un_tipo)? tipoH : un_tipo;
        string dirH1 = ci.nuevaTemporal();
        string d1 = ci.ampliar(dirH, tipoH, tipoH1);
        string d2 = ci.ampliar(puna.second, puna.first, tipoH1);
        /* dirS es ptp.second*/
        ci.genCod(oper, d1, d2, dirH1);
        pair<int, string> ptp = termP(tipoH1, dirH1);

        /* Regresamos los sintetizados. */
        return ptp;

    } else if (equals(tokenActual, MOD)){
        eat(MOD);
        pair<int, string> puna = unario();
        /* Acciones semánticas.*/
        if (!(tipoH==TIPO_INT && puna.first==TIPO_INT)){
            error("Tipos Incompatibles en operador '%'");
        }
        int termP1tipoH = TIPO_INT;
        string termP1dirH = ci.nuevaTemporal();
        ci.genCod("%", dirH, puna.second, termP1dirH);
        pair<int, string> ptp = termP(termP1tipoH, termP1dirH);
        /* Regresamos los sintetizados.*/
        return ptp;

    } else {
        /* Si no, es la producción era termP -> epsilon.*/
        /* Regresamos los heredados como sintetizados.*/
        return make_pair(tipoH, dirH);
    }
}

pair<int, string> unario() {
    if (equals(tokenActual, NOT)){
        eat(NOT);
        string una_dir = ci.nuevaTemporal();
        pair<int, string> un = unario();
        ci.genCod("!", "", un.second, una_dir);
        return make_pair(un.first, una_dir);
    } else if (equals(tokenActual, MENOS)){
        eat(MENOS);
        string una_dir = ci.nuevaTemporal();
        pair<int, string> un = unario();
        ci.genCod("-", "", un.second, una_dir);
        return make_pair(un.first, una_dir);
    } else { /* Si no, es la producción era unario -> factor.*/
        pair<int, string> fac = factor();
        /* Los sintetizados son de factor.*/
        return fac;
    }
}

pair<int, string> factor(){
    if (equals(tokenActual, PIZQ)){
        eat(PIZQ);
        pair<int, string> pb = bool_("", "");
        eat(PDER);
        /* Sintetizados obtenidos de bool*/
        return pb;
    } else if (equals(tokenActual, NUM)){
        Token *nume = eat(NUM);
        return make_pair(nume->tipo, nume->valor);
    } else if (equals(tokenActual, CADENA)){
        Token *chain = eat(CADENA);
        /*TablaCadenas.agregar(cadena.lexval) PARTE DE CADENAS---
        factor.dir = TablaCadenas.getUltimaPos()*/
        return make_pair(TIPO_CADENA, chain->valor);
    } else if (equals(tokenActual, _TRUE)){
        eat(_TRUE);
        return make_pair(TIPO_INT, "true");
    } else if (equals(tokenActual, _FALSE)){
        eat(_FALSE);
        return make_pair(TIPO_INT, "true");
    } else if (equals(tokenActual, ID)){
        /* Si no, puede ser la producción era factor -> localizacion
        * o la produccion factor -> id(parametros)
        */
        Token* base = eat(ID);
        pair<int, string> fp = factorP(base->valor);
        /* Regresamos los sintetizados de factorP.*/
        return fp;
    } else {
        /* Aquí no hay producción para epsilon, Si no fue ninguno de esos casos
         * tenemos un error. */
        error("Error de sintaxis\n");
        return make_pair(-1, ""); //dummy, no se alcanza.
    }
}

pair<int, string> factorP(string base) {
    if (equals(tokenActual, PIZQ)){
        eat(PIZQ);
        vector<int> l_p = parametros();
        eat(PDER);
        Simbolo* sbase = tablaSimbolosGlobal->busca(base);
        if (sbase != NULL) {
            if (sbase -> var == SFUNC){
                if (equivalentesListas(sbase->args, &l_p)){
                    int f_tipo = sbase->tipo;
                    string f_dir = ci.nuevaTemporal();
                    ci.genCod("call", base, to_string((sbase->args)->size()), f_dir);
                    return make_pair(f_tipo, f_dir);
                } else {
                    error("El número o tipo de parámetros no coinciden");
                }
            } else {
                error("El id no es de una función");
            }
        } else {
            error("El id no está declarado");
        }
        /*Dummy, no se llega, pero evita warning*/
        return make_pair(-1, "");
    } else if (equals(tokenActual, CIZQ)){
        /*FIRST localizacion = {"["}*/
        pair<int, string> ploc = localizacion(base);
        string factorPdir = ci.nuevaTemporal();
        /* Ponemos la indexación como operador. para la impresión, solo
         * ponemos la primer mitad y agregamos la segunda al fin de arg2.
         */
        ci.genCod("[", base, ploc.second + "]", factorPdir);
        return make_pair(ploc.first, factorPdir);
    } else {/*Si no, es la producción vacía.*/
        int fptipo = (pilaTS.top()->busca(base))->tipo;
        /* La base se vuelve dir*/
        return make_pair(fptipo, base);
    }
}

vector<int> parametros(){
    /* Si parametros -> epsilon
    * el siguiente símbolo es un paréntesis derecho.
    */
    if (!equals(tokenActual, PDER))
        return lista_param();
    else {
        return *(new vector<int>);
    }
}

vector<int> lista_param(){
    vector<int> listaH = *(new vector<int>);
    pair<int, string> pb = bool_("", "");
    listaH.push_back(pb.first);
    ci.genCod("param", pb.second, "", "");
    return lista_paramP(listaH);
}

vector<int> lista_paramP(vector<int>& listah){
    if (equals(tokenActual, COMA)){
        eat(COMA);
        pair<int, string> pb = bool_("", "");
        listah.push_back(pb.first);
        ci.genCod("param", pb.second, "", "");
        return lista_paramP(listah);
    } else {/* Si no teníamos lista_param -> epsilon. */
        return listah;
    }
}

/* Si no se ha consumido el ID de su producción, se consume.
 * Si ya se consumió lo indicamos con come == 0. Esto es
 * para hacer el backtracking (de un sólo token, por lo que no hay problema)
 * para saber en factor si la producción fue
 * factor -> localizacion o factor -> id(parametros)
 */
pair<int, string> localizacion(string base){
    if (equals(tokenActual, CIZQ)){
        eat(CIZQ);
        pair<int, string> pb = bool_("", "");
        /* Validamos que sea un entero*/
        if (pb.first != TIPO_INT) {
            error("El indice del arreglo debe ser entero");
        }
        eat(CDER);
        /* Inicio acciones semánticas*/
        TablaSimbolos* tsTop = pilaTS.top();
        Simbolo* buscado = tsTop->busca(base);
        /* Buscamos el tipo del id del arreglo*/
        if (buscado == NULL) {
            error("El id no está en el ambiente");
        }
        int tipoTmp_int = buscado->tipo;
        Tipo* tipoTmp = pilaTT.top()->getTipo(tipoTmp_int);
        /* Verificamos que sea un arreglo*/
        if (tipoTmp->tipo != "array") {
            error("El id no es un arreglo");
        }
        int locP_tipoH = tipoTmp -> tipoBase;
        Tipo *base = pilaTT.top()->getTipo(locP_tipoH);
        string locP_dirH = ci.nuevaTemporal();
        int locP_tam = base -> tam;
        ci.genCod("*", pb.second, to_string(locP_tam), locP_dirH);
        pair<int, string> pl = localizacionP(locP_tipoH, locP_dirH);
        return pl;
    } else{
        error("Error de Sintaxis\n");
        return make_pair(0, ""); //No se llega.
    }
}

pair<int, string>  localizacionP(int tipoH, string dirH){
    if (equals(tokenActual, CIZQ)) {
        eat(CIZQ);
        pair<int, string> pb = bool_("", "");
        eat(CDER);
        if (pb.first != TIPO_INT) {
            error("El indice del arreglo debe ser entero");
        }
        Tipo *tipoTmp = pilaTT.top()->getTipo(tipoH);
        if (tipoTmp == NULL || tipoTmp->tipo != "array") {
            error("El id no es un arreglo");
        }
        int locP1_tipoH = tipoTmp->tipoBase;
        Tipo *base = pilaTT.top()->getTipo(locP1_tipoH);
        string dirTmp = ci.nuevaTemporal();
        string locP1_dirH = ci.nuevaTemporal();
        int locP1_tam = base->tam;
        ci.genCod("*", pb.second, to_string(locP1_tam), dirTmp);
        ci.genCod("+", dirH, dirTmp, locP1_dirH);
        pair<int, string> sintetizados = localizacionP(locP1_tipoH, locP1_dirH);
        return sintetizados;
    } else {
        /* Si no teníamos localizacionP -> epsilon*/
        return make_pair(tipoH, dirH);
    }
}


void error(string msg) {
    printf("%s", msg.c_str());
    printf(": línea %i en %s\n", yylineno, (tokenActual->valor).c_str());
    exit(1);
}

void printTS(TablaSimbolos* ts){
    printf("\n=====================================\n");
    printf("    Tabla de Símbolos: %s           \n", (ts->nombreAmbiente).c_str());
    printf("=====================================\n");
    printf("id  \ttipo\tdir\tvar\targs\n");
    for (Simbolo* s : *(ts->tabla)){
        printf("%s\n", (s->printable()).c_str());
    }
}

void printTT(TablaTipos* tt){
    printf("\n=====================================\n");
    printf("    Tabla de Tipos: %s           \n", (tt->nombreAmbiente).c_str());
    printf("=====================================\n");
    printf("id  \ttipo\ttam\telem\tbase\n");
    for (Tipo* t : *(tt->tabla)){
        printf("%s\n", (t->printable()).c_str());
    }
}
