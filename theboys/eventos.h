#ifndef EVENTOS
#define EVENTOS
#include "entidades.h"

enum TipoEvento{
    CHEGA, ESPERA, DESISTE, AVISA, ENTRA,SAI, VIAJA, MORRE, MISSAO, FIM
};

struct Evento{
    int tempo;
    enum TipoEvento tipo;
    int id_heroi;
    int id_base;
    int id_missao;
};

void Chega (struct Mundo *M, struct Heroi *H, struct Base *B);

void Espera (struct Mundo *M, struct Heroi *H, struct Base *B);

void Desiste (struct Mundo *M, struct Heroi *H, struct Base *B);

void Avisa (struct Mundo *M, struct Base *B);

void Entra (struct Mundo *M, struct Heroi *H, struct Base *B);

void Sai (struct Mundo *M, struct Heroi *H, struct Base *B);

void Viaja (struct Mundo *M, struct Heroi *H, struct Base *D);

void Morre (struct Mundo *M, struct Heroi *H, struct Base *B,int id_missao);

void Missao (struct Mundo *M, int id_missao);

void Fim (struct Mundo *M);

#endif