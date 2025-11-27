#include "fila.h"
#include <stdio.h>
#include <stdlib.h>


// Cria uma fila vazia.
// Retorno: ponteiro para a fila criada ou NULL se erro.
struct fila_t *fila_cria (){
    struct fila_t *fila = calloc(1,sizeof(struct fila_t));
    if (!fila)
        return NULL;

    fila->prim = NULL;
    fila->fim = NULL;
    fila->num = 0;

    return fila;
}

// Libera todas as estruturas de dados da fila, inclusive os itens.
// Retorno: NULL.
struct fila_t *fila_destroi (struct fila_t *f){
    if (!f)
        return NULL;
    
    struct fila_nodo_t *aux = f->prim;
    while (aux != NULL){
        struct fila_nodo_t *remover = aux;
        aux = aux->prox;
        free(remover);
    }
    free(f);
    return NULL;
}

// Insere o item na fila
// Inserir duas vezes o mesmo item (o mesmo ponteiro) é um erro.
// Retorno: número de itens na fila após a operação ou -1 se erro.
int fila_insere (struct fila_t *f, void *item){
    if (!f)
        return -1;

    if (f->num == 0){
        struct fila_nodo_t *temp = malloc(sizeof(struct fila_nodo_t));
        f->prim = temp;
        f->fim = temp;
        f->num ++;
        temp->item = item;
        temp->prox = NULL;
        return f->num;
    }

    struct fila_nodo_t *aux = f->prim;
    while (aux != NULL){
        if (aux->item == item)
            return -1;
        aux = aux->prox;
    }

    struct fila_nodo_t *temp = malloc(sizeof(struct fila_nodo_t));
    temp->item = item;
    temp->prox = NULL;
    f->fim->prox = temp;
    f->fim = temp;
    f->num ++;

    return f->num;
}

// Retira o primeiro item da fila e o devolve
// Retorno: ponteiro para o item retirado ou NULL se fila vazia ou erro.
void *fila_retira (struct fila_t *f){
    if (!f || f->num == 0)
        return NULL;

    int *item = f->prim->item;
    struct fila_nodo_t *aux = f->prim->prox;
    free(f->prim);
    f->num --;
    f->prim = aux;

    return item;
}

// Informa o número de itens na fila.
// Retorno: N >= 0 ou -1 se erro.
int fila_tamanho (struct fila_t *f){
    if (!f)
        return -1;
    return f->num;
}

// Imprime o conteúdo da fila 
void fila_imprime (struct fila_t *f){
    if (f->num == 0)
        printf("LISTA VAZIA ");

    else{
        struct fila_nodo_t *temp = f->prim;
        while (temp->prox != NULL) {
            printf("%d ",*(int*)temp->item);
            temp = temp->prox;
        }
        printf("%d",*(int*)temp->item);
    }
}

