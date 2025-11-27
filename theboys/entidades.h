#ifndef ENTIDADES
#define ENTIDADES
#include "fila.h"
#include "conjunto.h"
#include "fprio.h"

struct Heroi{
    int id; // identifiação
    struct cjto_t *habilidades;  // possivelmente mudar para tipo conjunto
    int paciencia; // nivel de paciencia
    int velocidade; // nivel de velociadade
    int experiencia; // nivel de xp
    int base; // base em que está
    bool vivo; // vivo ou morto
};

struct Coordenada{
    int x; // coordenada x
    int y; // coordenada y
};

struct Base{
    int id; // identificaçao
    int lotacao; // num maximo que cabe
    struct cjto_t *presentes;  // Presentes na base (Usar o TAD conjunto)
    struct fila_t *espera; // Fila de espera
    struct Coordenada local; // local da base
    int fila_maxima; // fila maxima da base
    int missoes; // Numero de missoes que a base resolveu 
};

struct Mundo{
    int NHerois; // Quantidade de herois
    struct Heroi *Herois; // Vetor de herois
    int NBases; // Quantidade de bases
    struct Base *Bases; // Vetor de bases
    int NMissoes; //Quantidade de missoes
    struct Missao *Missoes; // Vetor de missoes
    int NHabilidades; // Quantidade de habilidades
    int NCompostosV; // Quantidade de CompostosV
    struct Coordenada TamanhoMundo; // Tamanho do mundo
    int Relogio; // Relogio global
    struct fprio_t *LEF; // FIla de eventos
    int Total_eventos; // Numero de eventos gerados
};

struct Missao{
    int id; // id da missao
    struct cjto_t *habilidades;  // Conjunto de habilidades necessarias 
    struct Coordenada local; // Local da missao
    bool concluida; // Concluida ou nao
    int tentativas; // numero de tentativas para resolver a missao
};;

#endif