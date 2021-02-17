#include <string>
#include <vector>
#include <stack>
#include <utility>
#include "tokens.h"
#include "semantico.h"
#include "codigointermedio.h"
#include "simbolo.h"
using namespace std;

/**
 * Función para paso de prueba y empate del No-terminal programa
 */
void programa();

/**
 * Función para paso de prueba y empate del No-terminal declaraciones
 */
void declaraciones();

/**
 * Función para paso de prueba y empate del No-terminal tipo.
 * @return un entero con el tipo básico que encontró, o -1 en
 * caso de que no encuentre ninguno. (t.tipo).
 */
int tipo();

/**
 * Función para paso de prueba y empate del No-terminal basico.
 * @return un como entero el tipo básico que encuentre o -1 en
 * caso de que no encuentre ninguno (basico.tipo).
 */
int basico();

/**
 * Función para paso de prueba y empate del No-terminal compuesto
 * @param b_tipo El atributo hereado de basico (basico.tipo o compuesto.tipo)
 * @return el tipo del compuesto.
 */
int compuesto(int base_tipo);

/**
 * Función para paso de prueba y empate del No-terminal lista_var
 * @param t_tipo el atributo heredado t.tipo.
 */
void lista_var(int t_tipo);

/**
 * Función para paso de prueba y empate del No-terminal lista_varP
 * que resulta de eliminación de la recursividad izquierda.
 * @oaram lv_tipo el atributo heredado lista_var.tipo.
 */
void lista_varP(int lv_tipo);

/**
 * Función para paso de prueba y empate del No-terminal funciones
 */
void funciones();

/**
 * Función para paso de prueba y empate del No-terminal argumentos
 */
vector<int>* argumentos();

/**
 * Función para paso de prueba y empate del No-terminal lista_args
 * @return una lista con los ids de los argumentos.
 */
vector<int>* lista_args();

/**
 * Función para paso de prueba y empate del No-terminal lista_argsP,
 * que es resultado de la eliminación de recursividad izquierda.
 * @param la lista heredada.
 * @return la lista sintetizada.
 */
vector<int>* lista_argsP(vector<int>* listaH);

/**
 * Función para paso de prueba y empate del No-terminal bloque
 */
void bloque(string sig);

/**
 * Función para paso de prueba y empate del No-terminal instrucciones
 * @param sig la etiqueta siguiente.
 */
void instrucciones(string sig);

/**
 * Función para paso de prueba y empate del No-terminal instruccionesP,
 * que es resultado de la eliminación de recursividad izquierda.
 * @param sig la etiqueta siguiente.
 */
void instruccionesP(string sig);

/**
 * Función para paso de prueba y empate del No-terminal sentencia.
 * @param sig la etiqueta siguiente.
 */
void sentencia(string sig);

/**
 * Función para paso de prueba y empate del No-terminal sentenciaP,
 * que es resultado de la eliminación de factores comunes.
 * @param sig una etiqueta
 * @param lista_indices con los indices a reemplazar.
 */
void sentenciaP(string sig, vector<string>& lista_indices);

/**
 * Función para paso de prueba y empate del No-terminal sentenciaPP,
 * que es resultado de la eliminación de factores comunes..
 */
void sentenciaPP();

/**
 * Función para paso de prueba y empate del No-terminal casos
 * @param id la variable del switch.
 * @param sig la etiqueta siguiente.
 * @param prueba las cuadruplas con código de las pruebas.
 */
vector<Cuadrupla*> casos(string id, string sig, vector<Cuadrupla*>& prueba);

/**
 * Función para paso de prueba y empate del No-terminal caso
 * @param id la variable probada.
 * @param sig la etiqueta siguiente
 * @return una cuadrupla para saltar al caso.
 */
Cuadrupla* caso(string id, string sig);

/**
 * Función para paso de prueba y empate del No-terminal predeterminado
 * @param sig la etiqueta siguiente.
 * @return una lista cuadrupla con el código para saltar al caso.
 */
Cuadrupla* predeterminado(string sig);

/**
 * Función para paso de prueba y empate del No-terminal parte_izquierda
 * @return un par con parte_izquierda.tipo y parte_izquierda.valor.
 */
pair<int, string> parte_izquierda();

/**
 * Función para paso de prueba y empate del No-terminal boolP,
 * que es resultado de la eliminación de factores izquierdos.
 * @param base atributo heredado con el valor del id. (izquierdaP.base)
 * @return un par de dir, tipo
 */
pair<int, string> parte_izquierdaP(string base);

/**
 * Función para paso de prueba y empate del No-terminal bool
 * @param vddr y fls dos etiquetas
 * @return un par con bool.tipo y bool.valor._
 */
pair<int, string>  bool_(string vddr, string fls);

/**
 * Función para paso de prueba y empate del No-terminal boolP,
 * que es resultado de la eliminación de recursividad izquierda.
 * @param tipoH el tipo heredado.
 * @param vddr, fls etiquetas heredadas.
 * @param lista_indices una lista con indices.
 * @param ft nos dice si fue llamado por primera vez (true) o en recursión.
 * @return una pareja tipo, dir.
 */
pair<int, string> boolP(int tipoH, string vddr, string fls, vector<string>& lista_indices, bool ft, string comb_fls);

/**
 * Función para paso de prueba y empate del No-terminal comb
 * @param vddr y fls etiquetas heredadas.
 * @lista_indices una lista con indices.
 * @return una pareja tipo, dir.
 */
pair<int, string> comb(string vddr, string fls);

/**
 * Función para paso de prueba y empate del No-terminal combP,
 * que es resultado de la eliminación de recursividad izquierda.
 * @param tipoH el tipo heredado.
 * @param vddr y fls etiquetas heredadas.
 * @param ft nos dice si fue llamado por primera vez (true) o en recursión.
 * @return una pareja tipo, dir.
 */
pair<int, string> combP(int tipoH, string vddr, string fls, vector<string>& lista_indices, bool ft);


/**
 * Función para paso de prueba y empate del No-terminal igualdad
 * @param vddr y fls etiquetas heredadas.
 * @return un par de tipo, dir.
 */
pair<int, string>  igualdad(string vddr, string fls);

/**
 * Función para paso de prueba y empate del No-terminal igualdadP,
 * que es resultado de la eliminación de recursividad izquierda.
 * @param tipoH tipo heredado.
 * @param dirH direccion heredada.
 * @param vddr y fls etiquetas heredadas.
 * @return un par de tipo, dir.
 */
pair<int, string>  igualdadP(int tipoH, string dirH, string vddr, string fls);

/**
 * Función para paso de prueba y empate del No-terminal rel
 * @param vddr y fls etiquetas heredadas.
 * @return un par de tipo, dir.
 */
pair<int, string>  rel(string vddr, string fls);

/**
 * Función para paso de prueba y empate del No-terminal relP,
 * que es resultado de la eliminación de factores izquierda.
 * @param tipoH tipo heredado.
 * @param dirH direccion heredada.
 * @param vddr y fls etiquetas heredadas.
 * @return un par de tipo, dir.
 */
pair<int, string>  relP(int tipoH, string dirH, string vddr, string fls);

/**
 * Función para paso de prueba y empate del No-terminal exp
 * @return un par de tipo, dir.
 */
pair<int, string> exp_();

/**
 * Función para paso de prueba y empate del No-terminal expP,
 * que es resultado de la eliminación de recursividad izquierda.
 * @param tipoH tipo heredado.
 * @param dirH direccion heredada.
 * @return un par de tipo, dir.
 */
pair<int, string>  expP(int tipoH, string dirH);

/**
 * Función para paso de prueba y empate del No-terminal term
 * @return un par de tipo, dir.
 */
pair<int, string>  term();

/**
 * Función para paso de prueba y empate del No-terminal termP,
 * que es resultado de la eliminación de recursividad izquierda.
 * @param tipoH tipo heredado.
 * @param dirH direccion heredada.
 * @return un par de tipo, dir.
 */
pair<int, string>  termP(int tipoH, string dirH);

/**
 * Función para paso de prueba y empate del No-terminal unario
 * @return un par de tipo, dir.
 */
pair<int, string> unario();

/**
 * Función para paso de prueba y empate del No-terminal factor
 * @return un par de tipo, dir.
 */
pair<int, string>  factor();

/**
 * Función para paso de prueba y empate del No-terminal factorP,
 * que es resultado de la eliminación de factores izquierdos.
 * @param base heredado del id.
 * @return un par de tipo, dir.
 */
pair<int, string> factorP(string base);

/**
 * Función para paso de prueba y empate del No-terminal parametros
 * @return un par de tipo, dir.
 */
vector<int> parametros();

/**
 * Función para paso de prueba y empate del No-terminal lista_param
 * @return una lista de tipos.
 */
vector<int>  lista_param();

/**
 * Función para paso de prueba y empate del No-terminal listaparamP,
 * que es resultado de la eliminación de recursividad izquierda.
 * @param listaH lista heredada.
 * @return un par de tipo, dir.
 */
vector<int>  lista_paramP(vector<int>& listaH);

/**
 * Función para paso de prueba y empate del No-terminal localizacion
 * recibe de parámetro un entero eat, que le indica si el id que
 * corresponde a su producción se tiene que consumir o ya o no.
 * Esto para hacer backtracking (en un sólo caso y con un sólo token).
 * @param base atributo heredado con el id de la parte_izquierda.
 * @return un par de tipo, dir.
 */
pair<int, string> localizacion(string base);

/**
 * Función para paso de prueba y empate del No-terminal localizacionP,
 * que es resultado de la eliminación de recursividad izquierda.
 * @param tipoH el tipo heredado.
 * @param dirH la dirección heredada.
 * @return un par de tipo, dir.
 */
pair<int, string>  localizacionP(int tipoH, string dirH);


/**
 * Función para consumo de componente léxico actual al hacer empate
 */
Token* eat(int clase);

/**
 * Función principal de análisis sintáctico.
 * Debe llamar a función de símbolo inicial de la gramática.
 * También inicializar las tablas de símbolos y tipos.
 */
void parse();

/**
 * Función para reporte de errores sintácticos y semánticos.
 */
void error(string msg);

/**
 * Función para imprimir Tabla de tipos
 */
void printTT(TablaTipos *TT);

/**
 * Función para imprimir Tabla de símbolos
 */
void printTS(TablaSimbolos *TS);
