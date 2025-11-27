
#include <stdio.h>
#include <stdlib.h>
#include "fprio.h"

// Cria uma fila vazia.
// Retorno: ponteiro para a fila criada ou NULL se erro.
struct fprio_t *fprio_cria (){
  struct fprio_t *fila = malloc(sizeof(struct fprio_t));
  if (!fila)
    return NULL;

  fila->num = 0;
  fila->prim = NULL;

  return fila;
}

// Libera todas as estruturas de dados da fila, inclusive os itens.
// Retorno: NULL.
struct fprio_t *fprio_destroi (struct fprio_t *f){
  if(!f)
    return NULL;

struct fpnodo_t *aux = f->prim;

  while (aux != NULL){
    struct fpnodo_t *remover = aux;
    aux = aux->prox;
    free(remover->item);
    free(remover);
  }
  free(f);
  return NULL;
}

// Insere o item na fila, mantendo-a ordenada por prioridades crescentes.
// Itens com a mesma prioridade devem respeitar a politica FIFO (retirar
// na ordem em que inseriu).
// Inserir duas vezes o mesmo item (o mesmo ponteiro) é um erro.
// Retorno: número de itens na fila após a operação ou -1 se erro.
int fprio_insere (struct fprio_t *f, void *item, int tipo, int prio){
  if (!f || !item) 
    return -1;

  struct fpnodo_t *aux = f->prim;
  while (aux != NULL) {
    if (aux->item == item)
      return -1; 
    aux = aux->prox;
  }

  struct fpnodo_t *nodo = malloc(sizeof(struct fpnodo_t));
  if (!nodo) return -1;

  nodo->item = item;
  nodo->tipo = tipo;
  nodo->prio = prio;
  nodo->prox = NULL;

  if (f->prim == NULL) {
    f->prim = nodo;
  }
  /*Coloca no inicio*/
  else if (prio < f->prim->prio) {
    nodo->prox = f->prim;
    f->prim = nodo;
  }
  /*Coloca meio/fim */
  else {
    struct fpnodo_t *atual = f->prim;
    while (atual->prox != NULL && atual->prox->prio <= prio)
      atual = atual->prox;
    nodo->prox = atual->prox;
    atual->prox = nodo;
  }

  f->num++;
  return f->num;
}

// Retira o primeiro item da fila e o devolve; o tipo e a prioridade
// do item são devolvidos nos parâmetros "tipo" e "prio".
// Retorno: ponteiro para o item retirado ou NULL se fila vazia ou erro.
void *fprio_retira (struct fprio_t *f, int *tipo, int *prio){
  if (!f || f->num == 0 || tipo == NULL || prio == NULL)
    return NULL;

  *tipo = f->prim->tipo;
  *prio = f->prim->prio;

  struct fpnodo_t *primeiro = f->prim;
  f->prim = f->prim->prox;
  f->num --;

  void *retorno = primeiro->item;
  free(primeiro);

  return retorno;
}

// Informa o número de itens na fila.
// Retorno: N >= 0 ou -1 se erro.
int fprio_tamanho (struct fprio_t *f){
  if (!f)
    return -1;
  return f->num;
}

// Imprime o conteúdo da fila no formato "(tipo prio) (tipo prio) ..."
// Para cada item deve ser impresso seu tipo e sua prioridade, com um
// espaço entre valores, sem espaços antes ou depois e sem nova linha.
void fprio_imprime (struct fprio_t *f){
  if (!f || f->prim == NULL) 
    return ;

  struct fpnodo_t *aux = f->prim;
  while (aux != NULL) {
    printf("(%d %d)", aux->tipo, aux->prio);
    if (aux->prox != NULL)
      printf(" ");
    aux = aux->prox;
  }
}
