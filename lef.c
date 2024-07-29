#include <stdio.h>
#include <stdlib.h>
#include "lef.h"

/* Cria um evento;
 * Retorna o ponteiro do eventou ou NULL em caso de falha */ 
struct evento_t *cria_evento (int tempo, int tipo, int dado1, int dado2) {

    struct evento_t *novo;
    
    novo = malloc (sizeof (struct evento_t));
    
    if (!novo)
        return NULL;
    
    novo->tempo = tempo;
    novo->tipo = tipo;
    novo->dado1 = dado1;
    novo->dado2 = dado2;

    return novo;
}


/* Destroi um evento e retorna NULL. */ 
struct evento_t *destroi_evento (struct evento_t *e) {

    free (e);
    e = NULL;
    
    return e;
}


/* Cria uma LEF vazia;
 * Retorna o ponteiro da LEF ou NULL em caso de falha. */ 
struct lef_t *cria_lef () {

    struct lef_t *cabeca;
    
    cabeca = malloc (sizeof (struct lef_t));
    
    if (!cabeca)
        return NULL;
        
    cabeca->primeiro = NULL;
    
    return cabeca;
}

/* Destroi a LEF, incluindo os seus eventos;
 * Retorna NULL. */ 
struct lef_t *destroi_lef (struct lef_t *l) {

    struct nodo_lef_t *aux, *lef;
    
    lef = l->primeiro;
    
    /* Elimina todos os nodos */
    while (lef != NULL) {
        aux = lef;
        lef = lef->prox;
        destroi_evento (aux->evento);
        free (aux);
    }
    
    /* Elimina nodo cabeca */
    free (l);
    l = NULL;
    
    return l;
}


/* Insere o evento em ordem crescente de tempo;
 * Em tempos iguais, respeita-se a politica FIFO;
 * Retorna 1 em caso de sucesso, 0 caso contrario. */
int insere_lef (struct lef_t *l, struct evento_t *e) {

    struct nodo_lef_t *novo_nodo, *aux;
    
    novo_nodo = malloc (sizeof (struct nodo_lef_t));
    
    if (!novo_nodo)
        return 0;
    
    novo_nodo->evento = e;
            
    /* Se fila estiver vazia */
    if (vazia_lef (l)) {
        novo_nodo->prox = NULL;
        l->primeiro = novo_nodo;
        return 1;
    }
    
        
    /* Insercao antes do primeiro elemento */
    if (l->primeiro->evento->tempo > e->tempo) {
        novo_nodo->prox = l->primeiro;
        l->primeiro = novo_nodo;
        return 1;
    }
    
    /* Encontra o nodo anterior daquele que se deve inserir */
    aux = l->primeiro;
    
    while ((aux->prox != NULL) && (aux->prox->evento->tempo <= e->tempo))
        aux = aux->prox;   
     
    novo_nodo->prox = aux->prox;
    aux->prox = novo_nodo;
    
    return 1;
}


/* Retira o primeiro evento da LEF;
 * Retorna o ponteiro do evento ou NULL em caso de falha. */
struct evento_t *retira_lef (struct lef_t *l) {

    struct nodo_lef_t *aux_nodo;
    struct evento_t *aux_evento;
    
    if (vazia_lef (l))
        return NULL;
        
    aux_evento = l->primeiro->evento;
    aux_nodo = l->primeiro;
    l->primeiro = l->primeiro->prox;
    free (aux_nodo);
    
    return aux_evento;
}


/* Retorna 1 se a LEF estiver vazia, 0 caso contrario. */
int vazia_lef (struct lef_t *l) {

    if (!l->primeiro)
        return 1;
    
    return 0;
}

/* Imprime os eventos da LEF (tempo tipo dado1 dado2)
 * e o total de eventos (total X eventos). */
void imprime_lef (struct lef_t *l) {

    int i;
    struct nodo_lef_t *aux;
    struct evento_t *e;
    
    i = 0;
    aux = l->primeiro;
    
    printf ("LEF:\n");
    
    while (aux != NULL) {
        e = aux->evento;
        printf ("  tempo %d tipo %d ", e->tempo, e->tipo);
        printf ("d1 %d d2 %d\n", e->dado1, e->dado2);
        aux = aux->prox;
        i++;
    }
    
    printf ("  Total %d eventos\n", i);
}
