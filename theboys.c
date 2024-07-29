#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fila.h"
#include "lef.h"
#include "conjunto.h"

#define T_INICIO 0
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS N_HABILIDADES * 5
#define N_BASES N_HEROIS / 6
#define N_MISSOES T_FIM_DO_MUNDO / 100

#define TIPO_CHEGA 0
#define TIPO_ESPERA 1
#define TIPO_DESISTE 2
#define TIPO_AVISA 3
#define TIPO_ENTRA 4
#define TIPO_SAI 5
#define TIPO_VIAJA 6
#define TIPO_MISSAO 7
#define TIPO_FIM 8

#define NULO -1


struct coordenada {
    int x;
    int y;
};


struct heroi {
    int ID;                     /* Identificação */
    struct conjunto *hab;       /* Habilidades */
    int pac;                    /* Paciencia */
    int vel;                    /* Velocidade */
    int exp;                    /* Experiencia */
    int base;                   /* ID da base que ele esta */
};

struct base {
    int ID;                     /* Identificacao */
    int lot;                    /* Lotacao */
    struct conjunto *presentes; /* Herois na base */
    struct fila *espera;        /* Fila para entrar */
    struct coordenada local;    /* Localizacao */
};


struct missao {
    int ID;                     /* Identificacao */
    struct conjunto *hab;       /* Habilidades Necessarias */
    struct coordenada local;    /* Localizacao */
};


struct world {
    int herois;                 /* Numero de Herois */
    struct heroi h[N_HEROIS];   /* Caracteristicas dos Herois */
    int bases;                  /* Numero de Bases */
    struct base b[N_BASES];     /* Caracteristicas das Bases */
    int missoes;                /* Numero de Missoes */
    struct missao m[N_MISSOES]; /* Caracteristicas das Missoes */
    int habilidades;            /* Numero de Habilidades */
    struct conjunto *hab;       /* Habilidades */
    int tam_mundo;              /* Tamanho do Mundo */
    int relogio;                /* Relogio */
    struct lef_t *timeline;     /* Lista de Eventos Futuros */
};


/* Retorna um numero aleatorio entre min e max, incluindo eles. */
int aleatorio (int min, int max) {

    return min + rand () % (max - min + 1);
}


/* Retorna a distancia entre duas coordenadas. */
int distancia_cartesiana (struct coordenada B, struct coordenada D) {

    int a, b;
    
    a = D.x - B.x;
    b = D.y - B.y;
    
    return sqrt (pow (a, 2) + pow (b, 2));
}


/* Retorna a posicao do menor elemento diferente de NULO ou
   NULO se todo o vetor for NULO. */
int busca_menor (int *v, int tam) {

    int i, pos_menor;
    
    /* Deduza-se que o menor elemento esteja na posicao 0. */
    pos_menor = 0;
    
    /* Garante que nao sejam consideradas as bases ja analisadas. */
    while ((pos_menor < tam) && (v[pos_menor] == NULO))
        pos_menor++;
    
    /* Se pos_menor = tam, o vetor inteiro eh NULO. */
    if (pos_menor == tam)
        return NULO;
    
    /*Garante que nao seja analisado elementos NULO antes de pos_menor*/
    for (i = pos_menor + 1; i < tam; i++)
        if ((v[i] != NULO) && (v[i] < v[pos_menor]))
            pos_menor = i;
    
    return pos_menor;    
}


/* Retorna uma coordenada aleatoria entre 0 e o tamanho do mundo. */
struct coordenada cria_coord (struct world *w) {

    struct coordenada c;
    
    c.x = aleatorio (0, w->tam_mundo - 1);
    c.y = aleatorio (0, w->tam_mundo - 1);
    
    return c;
}


/* Retorna um struct heroi com suas caracteristicas ja inicializadas. */
struct heroi cria_heroi (struct world *w, int ID) {

    struct heroi h;
    
    h.hab = cria_subcjt_cjt (w->hab, aleatorio (1, 3));
    h.ID = ID;
    h.pac = aleatorio (0, 100);
    h.vel = aleatorio (50, 5000);
    h.exp = 0;
    h.base = NULO;
    
    return h;
}
   

/* Retorna uma struct base com suas caracteristicas ja inicializadas. */
struct base cria_base (struct world *w, int ID) {

    struct base b;
    
    b.ID = ID;
    b.lot = aleatorio (3, 10);
    b.presentes = cria_cjt (b.lot);    
    b.espera = fila_cria ();
    b.local = cria_coord (w);
    
    return b;
}


/* Retorna uma struct missao com suas caracteristicas ja inicializadas.*/
struct missao cria_missao (struct world *w,  int ID) {

    struct missao m;
    
    m.ID = ID;
    m.hab = cria_subcjt_cjt (w->hab, aleatorio (6, 10));
    m.local = cria_coord (w);

    return m;
}


/* Cria os eventos de chegada dos herois, as missoes e o fim do mundo;
 * Insere os eventos na lef. */
void cria_eventos_iniciais (struct world *w) {

    int i, base, tempo;
    struct evento_t *ev;
   
    /* Chegada dos Herois */
    for (i = 0; i < w->herois; i++) { 
        base = aleatorio (0, w->bases - 1);
        tempo = aleatorio (0, 4320); /* 3 dias */
        ev = cria_evento (tempo, TIPO_CHEGA, i, base);
        insere_lef (w->timeline, ev); 
    };
    
    /* Missoes */
    for (i = 0; i < w->missoes; i++) {
        tempo = aleatorio (0, T_FIM_DO_MUNDO);
        ev = cria_evento (tempo, TIPO_MISSAO, i, NULO);
        insere_lef (w->timeline, ev);
    }
    
    /* Fim */
    ev = cria_evento (T_FIM_DO_MUNDO, TIPO_FIM, NULO, NULO);
    insere_lef (w->timeline, ev); 
}


/* Cria e retorna uma struct world;
 * Inicializa todas as suas caracteristicas; 
 * Cria uma lef e seus eventos iniciais. */
struct world cria_mundo () {

    int i;
    struct world w;
    
    w.tam_mundo = N_TAMANHO_MUNDO;
    w.relogio = T_INICIO;
    
    /* Habilidades */
    w.habilidades = N_HABILIDADES;
    w.hab = cria_cjt (w.habilidades);
    for (i = 0; i < w.habilidades; i++)
        insere_cjt (w.hab, i);
    
    /* Herois */
    w.herois = N_HEROIS;
    for (i = 0; i < w.herois; i++)
        w.h[i] = cria_heroi (&w, i);
    
    /* Bases */
    w.bases = N_BASES;
    for (i = 0; i < w.bases; i++)
        w.b[i] = cria_base (&w, i);
    
    /* Missoes */
    w.missoes = N_MISSOES;
    for (i = 0; i < w.missoes; i++)
        w.m[i] = cria_missao (&w, i);
    
    /* Lef e eventos */
    w.timeline = cria_lef ();
    cria_eventos_iniciais (&w);
    
    return w;
}


/* Destroi os herois, as bases, as missoes e o mundo. */
void destroi_mundo (struct world *w) {

    int i;
    
    /* Herois */
    for (i = 0; i < w->herois; i++)
        w->h[i].hab = destroi_cjt (w->h[i].hab);
    
    /* Bases */
    for (i = 0; i < w->bases; i++) {
        w->b[i].presentes = destroi_cjt (w->b[i].presentes);
        fila_destroi (&w->b[i].espera);
    }
    
    /* Missoes */
    for (i = 0; i < w->missoes; i++) 
        w->m[i].hab = destroi_cjt (w->m[i].hab);
    
    /* Mundo */    
    w->hab = destroi_cjt (w->hab);
    w->timeline = destroi_lef (w->timeline);
}


/* Atualiza a base do heroi;
 * Analisa se o heroi vai esperar ou desistir;
 * Informa a acao do heroi H em relacao a base B;
 * Cria um novo evento e o insere na lef. */
void evento_chega (struct world *w, int T, int H, int B) {

    int card_base, espera;
    struct heroi heroi;
    struct base base;
    struct evento_t *ev;
    
    heroi = w->h[H];
    base = w->b[B];
    card_base = cardinalidade_cjt (base.presentes);
    
    /* Atualiza base do heroi H */
    w->h[H].base = B;

    if ((card_base != base.lot) && (fila_vazia (base.espera)))
        espera = 1;
    else
        espera = heroi.pac > (10 * fila_tamanho (base.espera));
      
    if (espera) {
        ev = cria_evento (T, TIPO_ESPERA, H, B);
        insere_lef (w->timeline, ev);
        printf ("%6d: CHEGA  HEROI %2d BASE %d ", T, H, B);
        printf ("(%2d/%2d) ESPERA\n", card_base, base.lot);
        return;
    }
    
    ev = cria_evento (T, TIPO_DESISTE, H, B);
    insere_lef (w->timeline, ev);
    printf ("%6d: CHEGA  HEROI %2d BASE %d ", T, H, B);
    printf ("(%2d/%2d) DESISTE\n", card_base, base.lot);
}


/* Informa que o heroi H ira esperar para entrar na base B;
 * Coloca o heroi H na fila de espera de B;
 * Cria evento AVISA e o insere na lef. */
void evento_espera (struct world *w, int T, int H, int B) {

    struct evento_t *ev;
    
    printf ("%6d: ESPERA HEROI %2d BASE %d ", T, H, B);
    printf ("(%2d)\n", fila_tamanho (w->b[B].espera));
    
    enqueue (w->b[B].espera, H);
    
    ev = cria_evento (T, TIPO_AVISA, NULO, B);
    insere_lef (w->timeline, ev);
}


/* Informa que o heroi H desistiu de entrar na base B;
 * Cria evento VIAJA para uma base aleatoria e o insere na lef. */
void evento_desiste (struct world *w, int T, int H, int B) {

    int destino;
    struct evento_t *ev;
    
    printf ("%6d: DESIST HEROI %2d BASE %d\n", T, H, B);
    
    destino = aleatorio (0, w->bases - 1);
    ev = cria_evento (T, TIPO_VIAJA, H, destino);
    insere_lef (w->timeline, ev);
}


/* Informa que o porteiro da base B foi avisado;
 * Insere os herois da fila na base enquanto houver espaco;
 * Cria evento ENTRA para cada heroi admitido;
 * Insere os eventos na lef;
 * Informa que o heroi H foi admitido na base B. */
void evento_avisa (struct world *w, int T, int B) {

    int card_base, H;
    struct base base;
    struct evento_t *ev;
    
    base = w->b[B];
    card_base = cardinalidade_cjt (base.presentes);
    
    printf ("%6d: AVISA  PORTEIRO BASE %d ", T, B);
    printf ("(%2d/%2d) FILA ", card_base, base.lot);
    fila_imprime (base.espera);
    printf ("\n");    
    
    while ((card_base != base.lot) && (!fila_vazia (base.espera))) {
        dequeue (base.espera, &H);
        insere_cjt (base.presentes, H);
        ev = cria_evento (T, TIPO_ENTRA, H, B);
        insere_lef (w->timeline, ev);
        printf ("%6d: AVISA  PORTEIRO BASE %d ADMITE %2d\n", T, B, H);
        card_base = cardinalidade_cjt (base.presentes);
    }
}


/* Define o tempo de permanencia do heroi H na base B;
 * Cria evento SAI e o insere na lef;
 * Informa que o heroi H entrou na base B e quando ele ira sair. */
void evento_entra (struct world *w, int T, int H, int B) {

    int TPB; /* Tempo de Permanencia na Base */
    struct evento_t *ev;
    struct base base;
    
    base = w->b[B];
    
    TPB = 15 + w->h[H].pac * aleatorio (1, 20);
    
    ev = cria_evento (T + TPB, TIPO_SAI, H, B);
    insere_lef (w->timeline, ev);
    
    printf ("%6d: ENTRA  HEROI %2d BASE %d ", T, H, B);
    printf ("(%2d/%2d) ", cardinalidade_cjt (base.presentes), base.lot);
    printf ("SAI %d\n", ev->tempo);
}


/* Retira o heroi H da base B;
 * Define uma base destino;
 * Cria eventos VIAJA e AVISA e os insere na lef;
 * Informa a saida do heroi H da base B. */
void evento_sai (struct world *w, int T, int H, int B) {

    int destino;
    struct evento_t *ev;
    struct base base;
    
    base = w->b[B];
    
    retira_cjt (base.presentes, H);
    destino = aleatorio (0, w->bases - 1);
    
    ev = cria_evento (T, TIPO_VIAJA, H, destino);
    insere_lef (w->timeline, ev);
    
    ev = cria_evento (T, TIPO_AVISA, NULO, B);
    insere_lef (w->timeline, ev);
    
    printf ("%6d: SAI    HEROI %2d BASE %d ", T, H, B);
    printf ("(%2d/%2d)\n", cardinalidade_cjt (base.presentes), base.lot);
}
    
 
 /* Define a distancia e a duracao da viagem entre a base atual e a D;
  * Cria evento CHEGA e o insere na lef;
  * Informa que o heroi H vai viajar da base atual para a base D.  */
 void evento_viaja (struct world *w, int T, int H, int D) {
 
    int dist, duracao;
    struct evento_t *ev; 
    struct heroi heroi;
    struct base base_atual, base_dest;
    
    heroi = w->h[H];
    base_atual = w->b[heroi.base];
    base_dest = w->b[D];
    
    dist = distancia_cartesiana (base_atual.local, base_dest.local);
    duracao = dist / heroi.vel;
    
    ev = cria_evento (T + duracao, TIPO_CHEGA, H, D);
    insere_lef (w->timeline, ev); 
    
    printf ("%6d: VIAJA  HEROI %2d ", T, H);
    printf ("BASE %d BASE %d ", heroi.base, D);
    printf ("DIST %d VEL %d CHEGA %d\n", dist, heroi.vel, ev->tempo);
}


/* Analisa as habilidades dos herois de cada base;
 * Sera valida a base que possuir as habiliaddes necessarias para a
   missao M;
 * Se houver base valida, retorna sua ID. Se nao houver, retorna NULO. */
int encontra_base_valida (struct world *w, int M) {

    int i, menor, H;
    int v[w->bases];
    struct base base;
    struct conjunto *aux, *uniao;
    
    /* Cria vetor contendo as distancias entre uma base e a missao. */
    for (i = 0; i < w->bases; i++)
        v[i] = distancia_cartesiana (w->m[M].local, w->b[i].local);
    
    menor = busca_menor (v, w->bases);
    
    /* Se menor = NULO, todas as bases já foram analisadas */    
    while (menor != NULO) {
        base = w->b[menor];
        inicia_iterador_cjt (base.presentes);
        uniao = cria_cjt (w->habilidades);
        
        /* Cria um conjunto com as habilidades dos herois da base */
        while (incrementa_iterador_cjt (base.presentes, &H)) {
            aux = uniao;
            uniao = uniao_cjt (aux, w->h[H].hab);
            aux = destroi_cjt (aux);
        };
        
        /* Imprime as habilidades da base menor. */
        printf ("%6d: MISSAO %d ", w->relogio, M);
        printf ("HAB BASE %d: ", menor);
        imprime_cjt (uniao);
        
        /* Verifica se a base possui as habilidades necessarias. */
        if (contido_cjt (w->m[M].hab, uniao)) {
            uniao = destroi_cjt (uniao);
            return menor;
        }
        
        uniao = destroi_cjt (uniao);
        
        /*Garante que as bases invalidas nao sejam analisadas novamente*/
        v[menor] = NULO;        
        menor = busca_menor (v, w->bases);
    }
    
    return NULO;
} 


/* Informa as habilidades necessarias para a missao M;
 * Verifica se ha base valida;
 * Se nao houver base valida: informa MISSAO IMPOSSIVEL, cria evento
   MISSAO para o proximo dia e o insere na lef e retorna 0;
 * Se houver base valida: informa MISSAO CUMPRIDA, distribui experiencia
   aos herois e retorna 1. */
int evento_missao (struct world *w, int T, int M) {
    
    int base, H;
    struct evento_t *ev;
    struct missao missao;
    
    missao = w->m[M];
    
    printf ("%6d: MISSAO %d HAB REQ: ", T, M);
    imprime_cjt (missao.hab);
    
    base = encontra_base_valida (w, M);
    
    /* Se nao houver base valida. */
    if (base == NULO) {
        printf ("%6d: MISSAO %d IMPOSSIVEL\n", T, M);
        ev = cria_evento (T + 24 * 60, TIPO_MISSAO, M, NULO); 
        insere_lef (w->timeline, ev);
        return 0;
    }  
        
    /* Se houver base valida. */
    printf ("%6d: MISSAO %d CUMPRIDA ", T, M);
    printf ("BASE %d HEROIS: ", base);
    imprime_cjt (w->b[base].presentes);
    
    /* Da experiencia aos herois da base valida. */    
    inicia_iterador_cjt (w->b[base].presentes);
    while (incrementa_iterador_cjt (w->b[base].presentes, &H)) 
        w->h[H].exp++;
        
    return 1;           
}


/* Imprime as estatisticas dos herois e das missoes;
 * Destroi o mundo. */
void evento_fim (struct world *w, int m_cumpridas, int m_agendadas) {

    int i;
    float porcentagem, media;
    struct heroi h;
    
    printf ("%6d: FIM\n", w->relogio);
    
    /* Imprime as estatisticas dos herois. */
    for (i = 0; i < w->herois; i++)  {
        h = w->h[i];
        printf ("HEROI %2d PAC %3d ", h.ID, h.pac);
        printf ("VEL %4d EXP %4d HABS ", h.vel, h.exp);
        imprime_cjt (h.hab);
    }
    
    /* Calcula e imprime as estatisticas das missoes. */
    porcentagem = (float) (m_cumpridas * 100) / w->missoes;
    media = (float) m_agendadas / m_cumpridas;
    
    printf ("%d/%d MISSOES CUMPRIDAS ", m_cumpridas, w->missoes);
    printf ("(%.2f%%), ", porcentagem);
    printf ("MEDIA %.2f TENTATIVAS/MISSAO\n", media);
    
    destroi_mundo (w);
}


/* No evento AVISA, o dado1 eh nulo e o dado2, a base;
 * No evento MISSAO, o dado1 eh a ID e o dado2, NULO;
 * No evento CHEGA e VIAJA, o dado1 eh o heroi e o dado2, a base destino;
 * No evento FIM, tanto o dado1 como o dado2 sao NULO;
 * Nos demais eventos, o dado1 eh o heroi e o dado2 eh a base atual. */
int main () {

    int fim, m_agendadas, m_cumpridas;
    struct world w;
    struct evento_t *ev;
    
    srand (0);
    w = cria_mundo ();
    m_agendadas = w.missoes;    
    m_cumpridas = 0;
    fim = 0;
    
    while (!fim) {
        ev = retira_lef (w.timeline);
        w.relogio = ev->tempo;
        
        switch (ev->tipo) {
        
            case TIPO_CHEGA: 
                evento_chega (&w, ev->tempo, ev->dado1, ev->dado2);
                break;
                
            case TIPO_ESPERA:
                evento_espera (&w, ev->tempo, ev->dado1, ev->dado2);
                break;
                
            case TIPO_DESISTE:
                evento_desiste (&w, ev->tempo, ev->dado1, ev->dado2);
                break;
                
            case TIPO_AVISA:
                evento_avisa (&w, ev->tempo, ev->dado2);
                break;
                
            case TIPO_ENTRA:
                evento_entra (&w, ev->tempo, ev->dado1, ev->dado2);
                break;
                
            case TIPO_SAI:
                evento_sai (&w, ev->tempo, ev->dado1, ev->dado2);
                break;
                
            case TIPO_VIAJA:
                evento_viaja (&w, ev->tempo, ev->dado1, ev->dado2);
                break;
                
            case TIPO_MISSAO:
                if (evento_missao (&w, ev->tempo, ev->dado1))
                    m_cumpridas++;
                else
                    m_agendadas++;
                break;
                
            case TIPO_FIM: 
                fim = 1;
        }
        ev = destroi_evento (ev);
    }
    evento_fim (&w, m_cumpridas, m_agendadas);
};
