#include "simulador.h"

// --- Definição das Variáveis Globais ---
int NUM_FRAMES;
int TAMANHO_PAGINA;
int NUM_PROCESSOS;
Processo* PROCESSOS;
Frame* MEMORIA_FISICA;

int tempo_global = 0;
int proximo_frame_livre = 0;
int memoria_cheia = 0;
int ponteiro_clock = 0;
int num_page_faults = 0;
int num_acessos = 0;
// --- Fim da Definição de Variáveis Globais ---

void liberar_memoria() {
    int i;
    for (i = 0; i < NUM_PROCESSOS; i++) {
        if (PROCESSOS != NULL && PROCESSOS[i].tabela_paginas != NULL) {
            free(PROCESSOS[i].tabela_paginas);
        }
    }
    if (PROCESSOS != NULL) free(PROCESSOS);
    if (MEMORIA_FISICA != NULL) free(MEMORIA_FISICA);
}

void imprimir_resumo (const char *algoritmo){
    printf("\n--- Simulação Finalizada (Algoritmo: %s)\n", algoritmo);
    printf("Total de Acessos: %d\n", num_acessos);
    printf("Total de Page Faults: %d\n", num_page_faults);
}