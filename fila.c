#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

/* Cria nodo cabeca;
 * Retorna NULL caso nao tenha sido alocado. */
struct fila *fila_cria () {
    
    struct fila *f;
    
    f = malloc (sizeof (struct fila));
    
    if (!f)
        return NULL;
        
    f->ini = NULL;
    f->fim = NULL;
    f->tamanho = 0;
    
    return f;
}


/* Desaloca toda memoria da fila e faz fila receber NULL. */
void fila_destroi (struct fila **fila) {
    
    struct nodo *f, *aux;
    
    f = (*fila)->ini;
    
    while (f != NULL) {
        aux = f;
        f = f->prox;
        free (aux);
    }
    
    free (*fila);
    *fila = NULL;
}


/* Insere dado no fim da fila;
 * Retorna 0 caso nao tenha sido alocado, 1 caso contrario. */
int enqueue (struct fila *fila, int dado) {

    struct nodo *novo;
    
    novo = malloc (sizeof (struct nodo));
    
    if (!novo)
        return 0;
    
    novo->chave = dado;
    novo->prox = NULL;
    
    if (fila_vazia (fila)) 
        fila->ini = novo;
    else 
        fila->fim->prox = novo;
    
    fila->fim = novo;
    fila->tamanho++;
    
    return 1;
}


/* Remove o primeiro dado da fila;
 * Retorna o elemento no parametro dado;
 * Retorna 0 se fila vazia, 1 caso contrario. */
int dequeue (struct fila *fila, int *dado) {

    struct nodo *aux;
    
    if (fila_vazia (fila))
        return 0;
    
    aux = fila->ini;
    fila->ini = aux->prox;
    *dado = aux->chave;
    free (aux); 
        
    fila->tamanho--;
    
    return 1;
}

 
/* Retorna o numero de elementos da fila. */
int fila_tamanho (struct fila *fila) {

    return fila->tamanho;
}


/* Retorna 1 se a fila estiver vazia, 0 em caso contrario. */ 
int fila_vazia (struct fila *fila) {
    
    if (!fila->tamanho)
        return 1;
    
    return 0;
}


void fila_imprime (struct fila *fila) {

    struct nodo *aux;
    
    aux = fila->ini;
    printf ("[ ");
    
    /* Se fila vazia, aux = NULL */
    while (aux) {
        printf ("%d ", aux->chave);
        aux = aux->prox;
    }
    
    printf ("]");
}
