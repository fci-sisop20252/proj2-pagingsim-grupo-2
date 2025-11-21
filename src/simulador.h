#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Estruturas de Dados ---

typedef struct {
    int frame_id;           // ID do Frame físico (ou -1 se V=0)
    int valid_bit;          // 1 se na memória (HIT), 0 se PAGE FAULT
    int referenced_bit;     // R-bit: 1 se acessada, 0 caso contrário
    int tempo_chegada_fifo; // Para rastrear a ordem de chegada (FIFO)
} PageTableEntry;

typedef struct {
    int pid;                // PID do processo que ocupa o frame (-1 se livre)
    int pagina_id;          // Número da página virtual que está no frame
    int livre;              // 1 se livre, 0 se ocupado
} Frame;

typedef struct {
    int pid;
    int tamanho_virtual;
    int num_paginas;
    PageTableEntry* tabela_paginas; // Tabela de páginas específica do processo
} Processo;


// --- Variáveis Globais (Declaração 'extern') ---
// Estas variáveis são definidas em simulador.c e usadas em main.c
extern int NUM_FRAMES;
extern int TAMANHO_PAGINA;
extern int NUM_PROCESSOS;
extern Processo* PROCESSOS;
extern Frame* MEMORIA_FISICA;

// Variaveis de controle
extern int tempo_global;
extern int proximo_frame_livre;
extern int memoria_cheia;

//Variaveis de Algoritmo
extern int ponteiro_clock;

//Variaveis de resumo
extern int num_page_faults;
extern int num_acessos;


// --- Protótipos das Funções ---

// Funções de Inicialização e Limpeza
int ler_configuracao(char *filename);
void liberar_memoria();
void imprimir_resumo(const char *algoritmo);

// Função Principal de Simulação
void processar_acessos(const char *algoritmo, char *filename);
void tratar_hit(Processo *p, int pagina, int frame_id, int deslocamento);

// Funções de Tratamento de Page Fault
void tratar_page_fault(Processo *processo, int pagina, int deslocamento, const char *algoritmo);

// Funções de Alocação e Substituição
int encontrar_frame_livre();
int selecionar_vitima_fifo();
int selecionar_vitima_clock();

#endif // SIMULADOR_H
