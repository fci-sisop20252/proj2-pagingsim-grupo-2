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


// Função auxiliar para liberar a memória alocada
void liberar_memoria() {
    int i;
    // 1. Libera as tabelas de páginas de cada processo
    for (i = 0; i < NUM_PROCESSOS; i++) {
        if (PROCESSOS != NULL && PROCESSOS[i].tabela_paginas != NULL) {
            free(PROCESSOS[i].tabela_paginas);
        }
    }
    
    // 2. Libera o array de Processos
    if (PROCESSOS != NULL) {
        free(PROCESSOS);
    }
    
    // 3. Libera a Memória Física
    if (MEMORIA_FISICA != NULL) {
        free(MEMORIA_FISICA);
    }
}

// Implementação da função de leitura de configuração
int ler_configuracao(const char *filename){
    FILE *file_config;
    int i, j;

    file_config = fopen(filename, "r");
    if (file_config == NULL){
        perror("Erro ao abrir o arquivo de configuracao");
        return 0;
    }

    // Leitura dos parâmetros globais
    if(fscanf(file_config, "%d", &NUM_FRAMES) != 1 || 
       fscanf(file_config, "%d", &TAMANHO_PAGINA) != 1 || 
       fscanf(file_config, "%d", &NUM_PROCESSOS) != 1){
        fprintf(stderr, "Erro ao ler os parametros globais.\n");
        fclose(file_config);
        return 0;
    }

    // Aloca a memória física (Frames)
    MEMORIA_FISICA = (Frame*) malloc(NUM_FRAMES * sizeof(Frame));
    if (MEMORIA_FISICA == NULL){
        fprintf(stderr, "Erro ao alocar memoria fisica.\n");
        fclose(file_config);
        return 0;
    }
    // Inicializa frames como livres
    for (i = 0; i < NUM_FRAMES; i++) {
        MEMORIA_FISICA[i].pid = -1;
        MEMORIA_FISICA[i].pagina_id = -1;
        MEMORIA_FISICA[i].livre = 1;
    }

    // Aloca o array de processos
    PROCESSOS = (Processo*) malloc(NUM_PROCESSOS * sizeof(Processo));
    if (PROCESSOS == NULL){
        fprintf(stderr, "Erro ao alocar array de processos.\n");
        free(MEMORIA_FISICA);
        fclose(file_config);
        return 0;
    }

    // Le e inicializa os processos e tabelas
    for(i = 0; i < NUM_PROCESSOS; i++){
        int pid, tamanho_virtual;
        if(fscanf(file_config, "%d %d", &pid, &tamanho_virtual) != 2){
            fprintf(stderr, "Erro ao ler os parametros do processo %d.\n", i);
            liberar_memoria();
            fclose(file_config);
            return 0;
        }

        PROCESSOS[i].pid = pid;
        PROCESSOS[i].tamanho_virtual = tamanho_virtual;

        // Calcula o número de páginas virtuais
        PROCESSOS[i].num_paginas = tamanho_virtual / TAMANHO_PAGINA;
        
        // Aloca a tabela de páginas
        PROCESSOS[i].tabela_paginas = (PageTableEntry*) malloc(PROCESSOS[i].num_paginas * sizeof(PageTableEntry));
        if(PROCESSOS[i].tabela_paginas == NULL){
            fprintf(stderr, "Erro ao alocar a tabela de paginas para o processo %d.\n", pid);
            liberar_memoria();
            fclose(file_config);
            return 0;
        }

        // Inicializa a tabela de páginas
        for(j = 0; j < PROCESSOS[i].num_paginas; j++){
            PROCESSOS[i].tabela_paginas[j].frame_id = -1;
            PROCESSOS[i].tabela_paginas[j].valid_bit = 0;
            PROCESSOS[i].tabela_paginas[j].referenced_bit = 0;
            PROCESSOS[i].tabela_paginas[j].tempo_chegada_fifo = 0;
        }
    }

    fclose(file_config);
    return 1;
}

void tratar_hit(Processo *processo, int pagina, int frame_id, int deslocamento){
    processo->tabela_paginas[pagina].referenced_bit = 1;
    printf("Acesso: PID %d, Endereco %d (Pagina %d, Deslocamento %d) -> HIT: Pagina %d (PID %d) ja esta no Frame %d\n",
           processo->pid, 
           pagina * TAMANHO_PAGINA + deslocamento, 
           pagina, 
           deslocamento, 
           pagina, 
           processo->pid, 
           frame_id);
}

int encontrar_frame_livre(){
    int i;
    for (i = 0; i < NUM_FRAMES; i++){
        if (MEMORIA_FISICA[i].livre == 1){
            proximo_frame_livre = (i + 1) % NUM_FRAMES;
            return i;
        }
    }

    for(int i = 0; i < proximo_frame_livre; i++){
        if (MEMORIA_FISICA[i].livre == 1){
            proximo_frame_livre = (i + 1) % NUM_FRAMES;
            return i;
        }
    }

    return -1;
}

void imprimir_resumo (const char *algoritmo){
    printf("\n Simulação finalizada (Algoritmo: %s)\n", algoritmo);
    printf("Total de acessos: %d\n", num_acessos);
    printf("Total de page faults: %d\n", num_page_faults);
}
//FALTA FAZER: selecionar_vitima_fifo, selecionar_vitima_clock
//tratar_page_fault, processar_acessos, imprimir_resumo.
