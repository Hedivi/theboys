#include <stdio.h>
#include <stdlib.h>
#include "conjunto.h"


/* Retorna a posicao do elemento no conjunto;
 * Retorna 0 caso ele nao pertenca ao conjunto. */
int busca_binaria (struct conjunto *c, int elemento) {

    int ini, meio, fim;
    
    ini = 1;
    fim = c->card;
    meio = (ini + fim) / 2;
    
    while ((ini <= fim) && (elemento != c->v[meio])) {
        if (elemento < c->v[meio])
            fim = meio - 1;
        else 
            ini = meio + 1;
        meio = (ini + fim) / 2;
    }
    
    if (fim < ini)
        return 0;
    
    return meio;
}


/* Cria um conjunto vazio;
 * Retorna o ponteiro do conjunto ou NULL em caso de falha. */
struct conjunto *cria_cjt (int max) {

    struct conjunto *cjt;
    int *v;
    
    cjt = malloc (sizeof (struct conjunto));
    v = malloc ((max+1) * sizeof (int));
    
    if (!cjt || !v)
        return NULL;
    
    cjt->max = max;
    cjt->card = 0;
    cjt->v = v;
    inicia_iterador_cjt (cjt);
    
    return cjt;
}


/* Remove todos os elementos do conjunto;
 * Libera espaco;
 * Retorna NULL. */
struct conjunto *destroi_cjt (struct conjunto *c) {

    while (!vazio_cjt(c))
        retira_um_elemento_cjt (c);

    free (c->v);
    c->v = NULL;
    
    free (c);
    c = NULL;
    
    return c;
}


/* Retorna 1 se o conjunto estiver vazio, 0 caso contrario. */
int vazio_cjt (struct conjunto *c) {

    if (!c->card)
        return 1;
    
    return 0;
}


/* Retorna a cardinalidade do conjunto. */
int cardinalidade_cjt (struct conjunto *c) {

    return c->card;
}


/* Insere o elemento no conjunto sem que haja repeticoes; 
 * Retorna 1 em caso de sucesso ou se o elemento ja estiver no conjunto;
 * Retorna 0 se nao houver espaco. */
int insere_cjt (struct conjunto *c, int elemento) {

    int i;
    
    if (c->card == c->max)
        return 0;
    
    if (!pertence_cjt (c, elemento)) {
    
        c->v[0] = elemento; /* Sentinela */
        i = c->card;
        
        while (elemento < c->v[i]) {
            c->v[i+1] = c->v[i];
            i--;
        }
        
        c->v[i+1] = elemento;
        c->card++;
    }
    
    return 1;
}


/* Remove o elemento do conjunto caso ele exista;
 * Retorna 1 em caso de sucesso;
 * Retorna 0 caso o elemento nao pertenca ao conjunto. */
int retira_cjt (struct conjunto *c, int elemento) {

    int i, pos;
    
    pos = busca_binaria (c, elemento);
    
    if (pos) {
        for (i = pos; i < c->card; i++)
            c->v[i] = c->v[i+1];
        c->card--;    
        return 1;
    }
    
    return 0;
}


/* Retorna 1 se o elemento pertencer ao conjunto, 0 caso contrario. */
int pertence_cjt (struct conjunto *c, int elemento) {

    if (!busca_binaria (c, elemento))
        return 0;

    return 1;
} 



/* Retorna 1 se c1 estiver contido em c2, 0 caso contrario. */
int contido_cjt (struct conjunto *c1, struct conjunto *c2) {

    int i;
    
    if (c1->card > c2->card)
        return 0;
        
    for (i = 1; i <= c1->card; i++)
        if (!pertence_cjt (c2, c1->v[i]))
            return 0;
    
    return 1;
}


/* Retorna 1 se c1 for igual a c2, retorna 0 caso contrario;
 * Se eles tiverem a mesma cardinalidade e forem iguais,
 * um estara contido no outro. */
int sao_iguais_cjt (struct conjunto *c1, struct conjunto *c2) {

    if (c1->card != c2->card)
        return 0;
        
    return contido_cjt (c1, c2);
}


/* Cria um conjunto contendo a diferenca entre c1 e c2, respectivamente;
 * Retorna o ponteiro do conjunto ou NULL em caso de falha. */ 
struct conjunto *diferenca_cjt (struct conjunto *c1, struct conjunto *c2) {

    int i;
    struct conjunto *dif;
    
    /* No pior dos casos, c1 nao eh igual a c2, criando
     * um conjunto com a mesma cardinalidade de c1. */
    dif = cria_cjt (c1->card);
    
    if (!dif)
        return NULL;
    
    for (i = 1; i <= c1->card; i++) {
        if (!pertence_cjt (c2, c1->v[i]))
            insere_cjt (dif, c1->v[i]);
    }
    
    return dif;
}


/* Cria um conjunto contendo a interseccao entre c1 e c2;
 * Retorna o ponteiro do conjunto ou NULL em caso de falha. */
struct conjunto *interseccao_cjt (struct conjunto *c1, struct conjunto *c2) {

    int i;
    struct conjunto *intersec;
    
    /* No pior dos casos, o menor conjunto estara contido no maior,
     * criando um conjunto cuja cardinalidade eh igual a do menor */
    if (c1->card < c2->card)
        intersec = cria_cjt (c1->card);
    else
        intersec = cria_cjt (c2->card);
        
    if (!intersec)
        return NULL;
    
    for (i = 1; i <= c1->card; i++) {
        if (pertence_cjt (c2, c1->v[i]))
            insere_cjt (intersec, c1->v[i]);
    }
    
    return intersec;
}


/* Cria um conjunto contendo a uniao entre c1 e c2;
 * Retorna o ponteiro do conjunto ou NULL em caso de falha. */
struct conjunto *uniao_cjt (struct conjunto *c1, struct conjunto *c2) {

    int i;
    struct conjunto *uniao;
    
    /* No pior dos casos, c1 nao e igual a c2, criando um conjunto
     * cuja cardinalidade eh a soma das cardinalidades de c1 e c2. */ 
    uniao = cria_cjt (c1->card + c2->card);
    
    if (!uniao)
        return NULL;
    
    /* O conjunto uniao eh formado pelo c1 mais os
     * elementos de c2 que nao pertencem a c1. */
    for (i = 1; i <= c1->card; i++)
        uniao->v[i] = c1->v[i];
    
    uniao->card = c1->card;
    
    for (i = 1; i <= c2->card; i++) 
        if (!pertence_cjt (uniao, c2->v[i]))
            insere_cjt (uniao, c2->v[i]);
    
    return uniao;
}


/* Cria uma copia de c;
 * Retorna o ponteiro do conjunto ou NULL em caso de falha. */
struct conjunto *copia_cjt (struct conjunto *c) {

    int i;
    struct conjunto *copia;
    
    copia = cria_cjt (c->max);
    
    if (!copia)
        return NULL;
    
    copia->card = c->card;
    copia->ptr = c->ptr;
    
    for (i = 1; i <= copia->card; i++)
        copia->v[i] = c->v[i];

    return copia;
}


/* Cria um subconjunto com elementos de c;
 * Se n = 0, retorna um subconjunto vazio;
 * Se n >= cardinalidade de c, retorna uma copia de c;
 * Se n < cardinalidade de c, retorna um subconjunto
 * com n elementos aleatorios de c. */ 
struct conjunto *cria_subcjt_cjt (struct conjunto *c, int n) {

    int i, aleat;
    struct conjunto *subcjt;
    
    if (n >= c->card) {
        subcjt = copia_cjt (c);
        
        if (!subcjt)
            return NULL;
            
        return subcjt;
    }
    
    subcjt = cria_cjt (n);
    
    if (!subcjt)
        return NULL;
    
    if (n) 
        for (i = 0; i < n; i++) {
            aleat = 1 + rand () % (c->card);
            /* Garante que nao haja elementos repetidos */
            while (pertence_cjt (subcjt, c->v[aleat]))
                aleat = 1 + rand () % (c->card);
            insere_cjt (subcjt, c->v[aleat]);
        }
        
    return subcjt;        
}


/* Imprime os elementos do conjunto em ordem crescente. */
void imprime_cjt (struct conjunto *c) {
 
    int i;
        
    if (vazio_cjt (c)) {
        printf ("[ ]\n");
        return;
    }

    printf ("[ ");
    for (i = 1; i < c->card; i++) 
        printf ("%d ", c->v[i]);
        
    printf ("%d ]\n", c->v[c->card]);
}


/* Inicializa ptr usado na funcao incrementa_iterador */
void inicia_iterador_cjt (struct conjunto *c) {
    
    /*c->v[0] = sentinela */
    c->ptr = 1;
}


/* Devolve o elemento apontado em ret_iterador;
 * Incrementa o iterador;
 * Retorna 0 se o iterador ultrapassar o ultimo elemento;
 * Retorna 1 se o elemento estiver dentro do conjunto. */
int incrementa_iterador_cjt (struct conjunto *c, int *ret_iterador) {

    if (c->ptr > c->card)
        return 0;
        
    if (vazio_cjt (c))
        return 0;
        
    *ret_iterador = c->v[c->ptr];
    c->ptr++;

    return 1;
}


/* Um elemento aleatorio eh removido do conjunto;
 * Retorna o elemento removido. */
int retira_um_elemento_cjt (struct conjunto *c) {

    int i, pos, elemento;
    
    pos = 1 + rand () % (c->card);
    elemento = c->v[pos];
    
    for (i = pos; i < c->card; i++)
        c->v[i] = c->v[i+1];
    
    c->card--; 
    
    return elemento;
}
