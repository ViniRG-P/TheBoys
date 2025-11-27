#include "eventos.h"
#include "entidades.h"
#include "fprio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// funçoes auxiliares

// imprime as habilidades da missao resolvida
void Imprime_habilidades(struct Mundo *M, int id_base){
    struct cjto_t *uniao = cjto_cria(M->NHabilidades);

    for (int id_heroi = 0; id_heroi < M->NHerois; id_heroi++)
        if (M->Herois[id_heroi].vivo && cjto_pertence(M->Bases[id_base].presentes,id_heroi)){
            struct cjto_t *nova_uniao = cjto_uniao(uniao,M->Herois[id_heroi].habilidades);
            cjto_destroi(uniao);
            uniao = nova_uniao;
        }
    printf("[ ");
    cjto_imprime(uniao);
    printf(" ]\n");
    cjto_destroi(uniao);
}

// Incrementa o XP dos herois apos a missao
void Incrementa_xp(struct Mundo *M, int id_base){
    for (int id_heroi = 0; id_heroi < M->NHerois; id_heroi++)
        if (M->Herois[id_heroi].vivo && cjto_pertence(M->Bases[id_base].presentes,id_heroi))
            M->Herois[id_heroi].experiencia ++;
}

// Retorna o id do heroi com maior XP
int Heroi_maior_xp(struct Mundo *M,int id_base){
    int maior_experiencia = -1;
    int maior_experiencia_id = -1;

    for (int id_heroi = 0; id_heroi < M->NHerois; id_heroi++)
        if (M->Herois[id_heroi].vivo && cjto_pertence(M->Bases[id_base].presentes,id_heroi))
            if (M->Herois[id_heroi].experiencia > maior_experiencia){
                maior_experiencia = M->Herois[id_heroi].experiencia;
                maior_experiencia_id = id_heroi;
            }
    return maior_experiencia_id;
}

// Retorna true se a base for valida e false se for invalida
bool BaseValida(struct Mundo *M, int id_base, struct cjto_t *habilidades ){ 
    bool BaseValida = false;

    if (cjto_card(M->Bases[id_base].presentes) > 0){
        struct cjto_t *uniao = cjto_cria(M->NHabilidades);
        for (int id_heroi = 0; id_heroi < M->NHerois; id_heroi++)
            if (M->Herois[id_heroi].vivo && cjto_pertence(M->Bases[id_base].presentes,id_heroi)){
                struct cjto_t *nova_uniao = cjto_uniao(uniao,M->Herois[id_heroi].habilidades);
                cjto_destroi(uniao);
                uniao = nova_uniao;
            }

        BaseValida = cjto_contem(uniao,habilidades);
        cjto_destroi(uniao);
    }

    return BaseValida;
}

int Distancia(struct Mundo *M, struct Missao *missao, int i){

    struct Base *base = &M->Bases[i];

    int dx = base->local.x - missao->local.x;
    int dy = base->local.y - missao->local.y;

    int distancia = sqrt(dx * dx + dy * dy);

    return distancia;
}

// funcoes principais

void Chega (struct Mundo *M, struct Heroi *H, struct Base *B){
    H->base = B->id;
    bool espera = false;

    if (B->lotacao > cjto_card(B->presentes) && fila_tamanho(B->espera) == 0)
        espera = true;
    
    else    
        if (H->paciencia > 10 * fila_tamanho(B->espera))
            espera = true;

    struct Evento *evento = malloc(sizeof(struct Evento));

    if (!evento) {
        fprintf(stderr, "Erro: malloc falhou em CHEGA\n");
        exit(1);
    }

    evento->id_heroi = H->id;
    evento->id_base = B->id;
    evento->tempo = M->Relogio;
    evento->id_missao = -1;

    if (espera){
        evento->tipo = ESPERA;
        fprio_insere(M->LEF,evento,evento->tipo,evento->tempo);
        printf("%6d: CHEGA HEROI %2d BASE %d (%2d/%2d) ESPERA\n",evento->tempo,H->id,B->id,cjto_card(B->presentes),B->lotacao);
    }
    else{
        evento->tipo = DESISTE;
        fprio_insere(M->LEF,evento,evento->tipo,evento->tempo);
        printf("%6d: CHEGA HEROI %2d BASE %d (%2d/%2d) DESISTE\n",evento->tempo,H->id,B->id,cjto_card(B->presentes),B->lotacao);
    }
}

void Espera (struct Mundo *M, struct Heroi *H, struct Base *B){

    fila_insere(B->espera,&H->id);
    

    struct Evento *evento = malloc(sizeof(struct Evento));

    if (!evento) {
        fprintf(stderr, "Erro: malloc falhou em ESPERA\n");
        exit(1);
    }

    evento->id_heroi = H->id;
    evento->id_base = B->id;
    evento->tempo = M->Relogio;
    evento->tipo = AVISA;
    evento->id_missao = -1;

    fprio_insere(M->LEF,evento,evento->tipo,evento->tempo);
    printf("%6d: ESPERA HEROI %2d BASE %d (%2d)\n",evento->tempo,H->id,B->id,fila_tamanho(B->espera) - 1);

    if (fila_tamanho(B->espera) > B->fila_maxima)
        B->fila_maxima = fila_tamanho(B->espera);
}

void Desiste (struct Mundo *M, struct Heroi *H, struct Base *B){

    int aleatorio = rand() % M->NBases;

    struct Evento *evento = malloc(sizeof(struct Evento));

    if (!evento) {
        fprintf(stderr, "Erro: malloc falhou em DESISTE\n");
        exit(1);
    }
    
    evento->id_heroi = H->id;
    evento->id_base = aleatorio;
    evento->tempo = M->Relogio;
    evento->tipo = VIAJA;
    evento->id_missao = -1;

    fprio_insere(M->LEF,evento,evento->tipo,evento->tempo);
    printf("%6d: DESIST HEROI %2d BASE %d\n",evento->tempo,evento->id_heroi,B->id);
}

void Avisa (struct Mundo *M, struct Base *B){

    printf("%6d: AVISA PORTEIRO BASE %d (%2d/%2d) FILA [ ",M->Relogio,B->id,cjto_card(B->presentes),B->lotacao);
    fila_imprime(B->espera);
    printf(" ]\n");

    while (B->lotacao > cjto_card(B->presentes) && fila_tamanho(B->espera) > 0){
        int *id_heroi = fila_retira(B->espera);
        cjto_insere(B->presentes,*id_heroi);
        printf("%6d: AVISA PORTEIRO BASE %d ADMITE %2d\n",M->Relogio,B->id,*id_heroi);

        struct Evento *evento = malloc(sizeof(struct Evento));

        if (!evento) {
            fprintf(stderr, "Erro: malloc falhou em AVISA\n");
            exit(1);
        }
        
        evento->id_heroi = *id_heroi;
        evento->id_base = B->id;
        evento->tempo = M->Relogio;
        evento->tipo = ENTRA;
        evento->id_missao = -1;
    
        fprio_insere(M->LEF,evento,evento->tipo,evento->tempo);
    }
}

void Entra (struct Mundo *M, struct Heroi *H, struct Base *B){

    int TMB = 15 + H->paciencia * (rand() % (20) + 1);

    struct Evento *evento = malloc(sizeof(struct Evento));

    if (!evento) {
        fprintf(stderr, "Erro: malloc falhou em ENTRA\n");
        exit(1);
    }
    
    evento->id_heroi = H->id;
    evento->id_base = B->id;
    evento->tempo = M->Relogio + TMB;
    evento->tipo = SAI;
    evento->id_missao = -1;

    fprio_insere(M->LEF,evento,evento->tipo,evento->tempo);
    printf("%6d: ENTRA HEROI %2d BASE %d (%2d/%2d) SAI %d\n",evento->tempo,evento->id_heroi,evento->id_base,cjto_card(B->presentes),B->lotacao,evento->tempo);
}

void Sai (struct Mundo *M, struct Heroi *H, struct Base *B){

    cjto_retira(B->presentes,H->id);

    int aleatorio = rand() % M->NBases;

    
    struct Evento *evento = malloc(sizeof(struct Evento));

        if (!evento) {
            fprintf(stderr, "Erro: malloc falhou em SAI\n");
            exit(1);
        }
        
        evento->id_heroi = H->id;
        evento->id_base = aleatorio;
        evento->tempo = M->Relogio;
        evento->tipo = VIAJA;
        evento->id_missao = -1;
    
        fprio_insere(M->LEF,evento,evento->tipo,evento->tempo);
        printf("%6d: SAI HEROI %2d BASE %d (%2d/%2d)\n",evento->tempo,evento->id_heroi,B->id,cjto_card(B->presentes),B->lotacao);

    struct Evento *evento2 = malloc(sizeof(struct Evento));

    if (!evento2) {
        fprintf(stderr, "Erro: malloc falhou em SAI\n");
        exit(1);
    }
    
    evento2->id_heroi = H->id;
    evento2->id_base = B->id;
    evento2->tempo = M->Relogio;
    evento2->tipo = AVISA;

    fprio_insere(M->LEF,evento2,evento2->tipo,evento2->tempo);
}

void Viaja (struct Mundo *M, struct Heroi *H, struct Base *D){

    struct Base *origem = &M->Bases[H->base]; 

    int dx = D->local.x - origem->local.x;
    int dy = D->local.y - origem->local.y;

    int distancia = sqrt( dx * dx + dy * dy);

    int duracao = distancia / H->velocidade;

    struct Evento *evento = malloc(sizeof(struct Evento));

        if (!evento) {
            fprintf(stderr, "Erro: malloc falhou em VIAJA\n");
            exit(1);
        }
        
        evento->id_heroi = H->id;
        evento->id_base = D->id;
        evento->tempo = M->Relogio;
        evento->tipo = CHEGA;
        evento->id_missao = -1;
    
        fprio_insere(M->LEF,evento,evento->tipo,evento->tempo + duracao);
        printf("%6d: VIAJA HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n",evento->tempo,evento->id_heroi,origem->id,evento->id_base,distancia,H->velocidade
                                                                               ,evento->tempo+duracao);
}

void Morre (struct Mundo *M, struct Heroi *H, struct Base *B,int id_missao){
    cjto_retira(B->presentes,H->id);
    H->vivo = false;

    struct Evento *evento = malloc(sizeof(struct Evento));

    if (!evento) {
        fprintf(stderr, "Erro: malloc falhou em MORRE\n");
        exit(1);
    }
    
    evento->id_heroi = H->id;
    evento->id_base = H->base;
    evento->tempo = M->Relogio;
    evento->tipo = AVISA;
    evento->id_missao = -1;

    fprio_insere(M->LEF,evento,evento->tipo,evento->tempo);
    printf("%6d: MORRE HEROI %2d MISSAO %d\n",evento->tempo,evento->id_heroi,id_missao);
}

void Missao (struct Mundo *M, int id_missao){

    // Coloca um lugar aleatorio para a missao
    int x = rand() % M->TamanhoMundo.x;
    int y = rand() % M->TamanhoMundo.y;

    struct Missao *missao = &M->Missoes[id_missao];

    missao->local.x = x;
    missao->local.y = y;
    missao->tentativas ++;


    printf("%6d: MISSAO %d TENT %d HAB REQ: [ ",M->Relogio,missao->id,missao->tentativas);
    cjto_imprime(missao->habilidades);
    printf(" ]\n");

    int BMP = 999999;
    int BMP_indice = -1;
    // para o compostoV
    int menor_distancia = 999999;
    int menor_distancia_indice = -1;

    // Ve qual é a base mais proxima que consegue resolver a missao
    // Coloca a distancia em BMP e o indice em BMP_indice
    for (int id_base = 0; id_base < M->NBases; id_base++){
        int distancia = Distancia(M,missao,id_base);
        if (BaseValida(M,id_base,missao->habilidades))
            if (distancia < BMP){
                BMP = distancia;
                BMP_indice = id_base;
        }
        if (cjto_card (M->Bases[id_base].presentes) > 0 && distancia < menor_distancia){
            menor_distancia = distancia;
            menor_distancia_indice = id_base;
        }
    }

    // Se não achou base que tenham as habilidades    
    if (BMP == 999999)
        // Testa para usar componenteV
        if (M->NCompostosV > 0 && menor_distancia_indice != -1 && M->Relogio % 2500 == 0) {
            int heroiXP = Heroi_maior_xp(M,menor_distancia_indice);
            if (heroiXP != -1){
                struct Evento *evento = malloc(sizeof(struct Evento));

                if (!evento) {
                    fprintf(stderr, "Erro: malloc falhou em MISSAO\n");
                    exit(1);
                }
                
                missao->concluida = true;
                M->Bases[menor_distancia_indice].missoes ++;
                Incrementa_xp(M,menor_distancia_indice);
                M->NCompostosV --;
                evento->id_heroi = heroiXP;
                evento->id_base = menor_distancia_indice;
                evento->tempo = M->Relogio;
                evento->tipo = MORRE;
                evento->id_missao = -1;

                fprio_insere(M->LEF,evento,evento->tipo,evento->tempo);
            }
            else
                goto missao_impossivel;
        }
        else{
            // missao impossivel
            missao_impossivel:
            printf("%6d: MISSAO %d IMPOSSIVEL\n",M->Relogio,missao->id);
            struct Evento *evento = malloc(sizeof(struct Evento));

            if (!evento) {
                fprintf(stderr, "Erro: malloc falhou em MISSAO\n");
                exit(1);
            }
            
            evento->tipo = MISSAO;
            evento->id_base = -1;
            evento->tempo = M->Relogio + 1440;
            evento->id_missao = id_missao;

            fprio_insere(M->LEF,evento,evento ->tipo,evento->tempo);
        }
    // Achou a base com habilidades
    else{

        printf("%6d: MISSAO %d CUMPRIDA BASE %d HABS: ",M->Relogio,missao->id,BMP_indice);
        Imprime_habilidades(M,BMP_indice);

        Incrementa_xp(M,BMP_indice);
        missao->concluida = true;
        M->Bases[BMP_indice].missoes ++;
    }
}

void Fim (struct Mundo *M){
    printf("\n525600: FIM\n\n");

    int num_mortos = 0;
    for (int id_heroi = 0; id_heroi < M->NHerois; id_heroi++){
        int pac = M->Herois[id_heroi].paciencia;
        int vel = M->Herois[id_heroi].velocidade;
        int exp = M->Herois[id_heroi].experiencia;
        if (M->Herois[id_heroi].vivo){
            printf("HEROI %2d VIVO PAC %3d VEL %4d EXP %4d HABS [ ",id_heroi,pac,vel,exp);
            cjto_imprime(M->Herois[id_heroi].habilidades);
            printf(" ]\n");
        }
        else{
            num_mortos ++;
            printf("HEROI %2d MORTO PAC %3d VEL %4d EXP %4d HABS [ ",id_heroi,pac,vel,exp);
            cjto_imprime(M->Herois[id_heroi].habilidades);
            printf(" ]\n");
        }
    }

    for (int id_base = 0; id_base < M->NBases; id_base++){
        int Fila_max =M->Bases[id_base].lotacao;
        int missoes =M->Bases[id_base].missoes;
        printf("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n",id_base,M->Bases[id_base].lotacao,Fila_max,missoes);
    }

    printf("EVENTOS TRATADOS: %d\n",M->Total_eventos);

    int missoes_concluidas = 0;

    int menor_tentativa = 999;
    int maior_tentativa = 0;
    int total_tentativas = 0;

    for (int id_missao = 0; id_missao < M->NMissoes; id_missao++){
        if (M->Missoes[id_missao].concluida)
            missoes_concluidas ++;
        if (M->Missoes[id_missao].tentativas < menor_tentativa)
            menor_tentativa = M->Missoes[id_missao].tentativas;
        if (M->Missoes[id_missao].tentativas > maior_tentativa)
            maior_tentativa = M->Missoes[id_missao].tentativas;
        total_tentativas += M->Missoes[id_missao].tentativas;
    }
    printf("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n",missoes_concluidas,M->NMissoes,(double)missoes_concluidas/M->NMissoes * 100);

    printf("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n",menor_tentativa,maior_tentativa,(double)total_tentativas/M->NMissoes);

    printf("TAXA MORTALIDADE: %.1f%%\n",(double)num_mortos / M->NHerois * 100);
}
