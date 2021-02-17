%{
  #include <stdio.h>
  #include "tokens.h"
  #define YY_DECL Token *yylex(void)
%}

%option noyywrap
%option yylineno

    /* Para estos tokens retomamos lo que teníamos de
     * la parte anterior del proyecto.
     *
     * Identificadores.
     * Deben iniciar con una letra y están formados
     * por letras y digitos.
     */
id [a-zA-Z][a-zA-Z0-9]*

    /* Números enteros.
     * Pueden estar separados por un guión bajo.
     */
entero_sencillo [0-9]+
num_ent ({entero_sencillo}_)*{entero_sencillo}

    /* Números de punto flotante.
     * Notación con punto: La parte antes del punto puede tener _
     * Podemos tener un entero con punto y seguirlo o no con números
     * después del punto; si no hay entero antes, debe haber números
     * después del punto.
     * Notación científica: número entero o decimal seguido de un
     * exponente definido como una e seguida de un signo y un entero.
     */
con_punto ({num_ent}\.{num_ent}?|\.{num_ent})
exp [eE][+\-]?{num_ent}
num_real ({con_punto}{exp}?|{num_ent}{exp})
    /* Los floats deben terminar en [fF]*/
num_float {num_real}[fF]
    /* Los doubles no. */
num_dble {num_real}

    /* Sólo faltan las cadenas. (Ya no hay imaginarios y los booleanos
     * son sólo palabras reservadas).
     * Dentro de la cadena puede ir cualquier caracter que no
     * rompa la cadena. En los ejemplos se ve que incluir un salto de
     * línea también es válido (pero sólo si hay comillas simples).
     * En las dobles podemos "escapar" el salto de línea \\n
     */
dentro_cadena [^"'\n]
    /* En las simples puede haber dentro un salto de línea y
     * comillas dobles, pero las simples deben ir escapadas \\'
     */
simples '({dentro_cadena}|\\'|\"|\n)*'
    /* En las dobles puede haber dentro un salto de línea
     * pero escapado \\n y comillas simples.
     * Las comillas dobles, deben ir escapadas \\\"
     */
dobles  \"({dentro_cadena}|'|\\\"|\\n)*\"
cadena ({dobles}|{simples})

char '.'|'\\n'|'\\t'|'\''

slinecomment --[^\n]*
mlinecomment "<*"([^*]|\*[^>])*"*>"

whitespace [ \t\n]

%%

 /*Caracteres Especiales*/
"," { return crea_token(COMA, yytext); }
";" { return crea_token(PCOMA, yytext); }
":" { return crea_token(DOSPUNTOS, yytext); }
"[" { return crea_token(CIZQ, yytext); }
"]" { return crea_token(CDER, yytext); }
"(" { return crea_token(PIZQ, yytext); }
")" { return crea_token(PDER, yytext); }
"{" { return crea_token(LIZQ, yytext); }
"}" { return crea_token(LDER, yytext); }
 /* Tipos */
"int"   { return crea_token(INT, yytext); }
"float" { return crea_token(FLOAT, yytext);}
"char"  { return crea_token(CHAR, yytext);}
"double" { return crea_token(DOUBLE, yytext);}
"void"  { return crea_token(VOID, yytext);}
 /* Operadores */
"="  { return crea_token(ASIG, yytext); }
"!"  { return crea_token(NOT, yytext); }
"&&" { return crea_token(AND, yytext); }
"||" { return crea_token(OR, yytext); }
"==" { return crea_token(EQ, yytext); }
"!=" { return crea_token(NEQ, yytext); }
"<"  { return crea_token(MENOR, yytext); }
">"  { return crea_token(MAYOR, yytext); }
"<=" { return crea_token(MENOROIGUAL, yytext); }
">=" { return crea_token(MAYOROIGUAL, yytext); }
"+"  { return crea_token(MAS, yytext); }
"-"  { return crea_token(MENOS, yytext); }
"*"  { return crea_token(POR, yytext); }
"/"  { return crea_token(DIV, yytext); }
"%"  { return crea_token(MOD, yytext); }
 /* Palabras Reservadas */
"func"  { return crea_token(FUNC, yytext); }
"if"    { return crea_token(_IF, yytext); }
"else"  { return crea_token(_ELSE, yytext); }
"while" { return crea_token(WHILE, yytext); }
"do"    { return crea_token(DO, yytext); }
"break" { return crea_token(BREAK, yytext); }
"switch" { return crea_token(SWITCH, yytext); }
"case"  { return crea_token(CASE, yytext); }
"default" { return crea_token(DEFAULT, yytext); }
"true"  { return crea_token(_TRUE, yytext);}
"false" { return crea_token(_FALSE, yytext);}
"return" { return crea_token(RETURN, yytext);}
"print" { return crea_token(PRINT, yytext);}
"scan"  { return crea_token(SCAN, yytext);}

 /* Literales. Deben ir al final pues id captura patrones con conflictos. */
{id}        { return crea_token(ID, yytext); }
{num_ent}   { return crea_token_tipo(NUM, yytext, 1); }
{num_float} { return crea_token_tipo(NUM, yytext, 2); }
{num_dble}  { return crea_token_tipo(NUM, yytext, 3); }
{char}      { return crea_token_tipo(NUM, yytext, 4); }
{cadena}    { return crea_token(CADENA, yytext); }

 /* Comentarios */
{slinecomment} {/*Ignoramos los comentarios*/}
{mlinecomment} {/*Ignoramos los comentarios*/}
 /* Otros */
{whitespace} { }
<<EOF>> { puts("Fin de análisis léxico."); return crea_token(FIN, ""); }
. { printf("Error léxico en la linea: %i\n", yylineno); exit(1); }

%%
