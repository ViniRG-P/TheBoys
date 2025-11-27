
// Autor: Vinicius Garcia, GRR 20255453

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "entidades.h"
#include "eventos.h"

#define T_FIM_DO_MUNDO 525600 

// Inicicaliza um heroi
void Inicializa_heroi(struct Heroi *H, int id, int num_habilidades){

  H->id = id;
  H->habilidades = cjto_aleat(rand() % 3 + 1, num_habilidades);
  H->paciencia = rand() % 101 + 0;
  H->velocidade = rand() % 4951 + 50;
  H->experiencia = 0;
  H->base = -1;
  H->vivo = true;
}

// Inicializa uma base
void Inicializa_base(struct Base *B, int id, int tam_mundo, int num_herois){

  B->id = id;
  B->lotacao = rand() % 8 + 3 ;
  B->presentes = cjto_cria(num_herois);
  B->espera = fila_cria();
  B->local.x = rand() % tam_mundo;
  B->local.y = rand() % tam_mundo;
  B->fila_maxima = 0;
  B->missoes = 0;
}

// Inicializa uma missao
void Inicializa_missao(struct Missao *W,int id, int num_habilidades, int tam_mundo){
    W->id = id;
    W->habilidades = cjto_aleat(rand() %5 + 6,num_habilidades);  // Conjunto de habilidades necessarias (Usar o TAD conjunto) 
    W->local.x = rand() % tam_mundo;
    W->local.y = rand() % tam_mundo;
    W->concluida = false;
    W->tentativas = 0;
}

// Inicializa o mundo, bases, missoes e herois
void Iniciaiza_mundo(struct Mundo *M){

  const int N_TAMANHO_MUNDO = 20000;
  const int N_HABILIDADES = 10;

  M->NHerois = N_HABILIDADES * 5;
  M->NBases = M->NHerois / 5;
  M->NCompostosV = N_HABILIDADES * 3;
  M->NMissoes = T_FIM_DO_MUNDO / 100;
  M->Relogio = 0;
  M->NHabilidades = N_HABILIDADES;
  M->LEF = fprio_cria();
  M->Total_eventos = 0;
  M->TamanhoMundo.x = N_TAMANHO_MUNDO;
  M->TamanhoMundo.y = N_TAMANHO_MUNDO;

  M->Herois = malloc(M->NHerois * sizeof(struct Heroi));
  M->Bases = malloc(M->NBases * sizeof(struct Base));
  M->Missoes = malloc(M->NMissoes * sizeof(struct Missao));

  for (int i = 0; i < M->NHerois; i ++)
    Inicializa_heroi(&M->Herois[i],i,N_HABILIDADES);

  for (int j = 0; j < M->NBases; j ++)
    Inicializa_base(&M->Bases[j],j,N_TAMANHO_MUNDO,M->NHerois);

  for (int k = 0; k < M->NMissoes; k ++)
    Inicializa_missao(&M->Missoes[k],k,N_HABILIDADES,N_TAMANHO_MUNDO);
}
// inicializa os herois na base e o evento FIM e coloca missoes iniciais na LEF
void Inicializa_eventos_iniciais(struct Mundo *M){
  for (int id_heroi = 0; id_heroi < M->NHerois; id_heroi++){
    int id_base = rand() % M->NBases;
    M->Herois[id_heroi].base = id_base;
    int tempo = rand() % 4321 + 0;

    struct Evento *evento = malloc(sizeof(struct Evento));

    if (!evento) {
      fprintf(stderr, "Erro: malloc falhou em INICIALIZA\n");
      exit(1);
    }

    evento->tipo = CHEGA;
    evento->id_heroi = id_heroi;
    evento->id_base = id_base;
    evento->id_missao = -1;
    evento->tempo = tempo;

    fprio_insere(M->LEF,evento,evento->tipo,evento->tempo);
  }

  for (int id_missao = 0; id_missao < M->NMissoes; id_missao ++){
    struct Evento *missao = malloc(sizeof(struct Evento));

    if (!missao) {
      fprintf(stderr, "Erro: malloc falhou em INICIALIZA\n");
      exit(1);
    }

    missao->tipo = MISSAO;
    missao->id_heroi = -1;
    missao->id_base = -1;
    missao->id_missao = id_missao;
    missao->tempo = rand() % T_FIM_DO_MUNDO + 0;

    fprio_insere(M->LEF,missao,missao->tipo,missao->tempo);
  }

  struct Evento *fim = malloc(sizeof(struct Evento));

    if (!fim) {
      fprintf(stderr, "Erro: malloc falhou em INICIALIZA\n");
      exit(1);
    }

    fim->tipo = FIM;
    fim->id_heroi = -1;
    fim->id_base = -1;
    fim->id_missao = -1;
    fim->tempo = T_FIM_DO_MUNDO;

    fprio_insere(M->LEF,fim,fim->tipo,fim->tempo);
}

void Destruir_o_mundo(struct Mundo *M){

  for (int id_heroi = 0; id_heroi < M->NHerois; id_heroi ++)
    cjto_destroi(M->Herois[id_heroi].habilidades);
  free(M->Herois);

  for (int id_base = 0; id_base < M->NBases; id_base ++){
    cjto_destroi(M->Bases[id_base].presentes);
    fila_destroi(M->Bases[id_base].espera);
  }
  free(M->Bases);

  for (int id_missao = 0; id_missao < M->NMissoes; id_missao ++)
    cjto_destroi(M->Missoes[id_missao].habilidades);
  free(M->Missoes);
  
  fprio_destroi(M->LEF);
}

int main (){
  srand(time(NULL));

  struct Mundo M;
  Iniciaiza_mundo(&M);

  Inicializa_eventos_iniciais(&M);

  bool fim_simulacao = false;
  while (fprio_tamanho(M.LEF) > 0 && !fim_simulacao){
    int tipo,prio;
    struct Evento *evento = (struct Evento *)fprio_retira(M.LEF,&tipo,&prio);

    M.Relogio = evento->tempo;
    M.Total_eventos ++;

    switch (evento->tipo){
    case CHEGA:
      Chega(&M,&M.Herois[evento->id_heroi],&M.Bases[evento->id_base]);
      break;
    case ESPERA:
      Espera(&M,&M.Herois[evento->id_heroi],&M.Bases[evento->id_base]);
      break;
    case DESISTE:
      Desiste(&M,&M.Herois[evento->id_heroi],&M.Bases[evento->id_base]);
      break;
    case AVISA:
      Avisa(&M,&M.Bases[evento->id_base]);
      break;
    case ENTRA:
      Entra(&M,&M.Herois[evento->id_heroi],&M.Bases[evento->id_base]);
      break;
    case SAI:
      Sai(&M,&M.Herois[evento->id_heroi],&M.Bases[evento->id_base]);
      break;
    case VIAJA:
      Viaja(&M,&M.Herois[evento->id_heroi],&M.Bases[evento->id_base]);
      break;
    case MORRE:
      Morre(&M,&M.Herois[evento->id_heroi],&M.Bases[evento->id_base],evento->id_missao);
      break;
    case MISSAO:
      Missao(&M,evento->id_missao);
      break;
    case FIM:
      Fim(&M);
      fim_simulacao = true;
      break;
    default:
      break;
    }
    free(evento);
  }

  Destruir_o_mundo(&M);

  return (0) ;
}

