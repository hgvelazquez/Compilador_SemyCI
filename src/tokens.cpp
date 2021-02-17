#include "tokens.h"

Token *crea_token(int clase, string valor) {  
  Token *t_out = new Token(clase, valor, 0);
  return t_out;
}

Token *crea_token_tipo(int clase, string valor, int tipo) {
  Token *t_out = new Token(clase, valor, tipo);
  return t_out;
}

int equals(Token *t1, int clase) {
  return t1->clase == clase;
}

void libera(Token *t){
    delete t;
}
