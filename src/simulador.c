// simulador.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para a Entrada da Tabela de Páginas (PTE)
typedef struct {
    int frame_id;           // ID do Frame físico (ou -1 se V=0)
    int valid_bit;          // 1 se na memória (HIT), 0 se PAGE FAULT
    int referenced_bit;     // R-bit: 1 se acessada, 0 caso contrário
    int tempo_chegada_fifo; // Para rastrear a ordem de chegada (FIFO)
} PageTableEntry;

// Estrutura do Frame Físico
typedef struct {
    int pid;                // PID do processo que ocupa o frame (-1 se livre)
    int pagina_id;          // Número da página virtual que está no frame
    int livre;              // 1 se livre, 0 se ocupado
} Frame;

// Estrutura do Processo
typedef struct {
    int pid;
    int tamanho_virtual;
    int num_paginas;
    PageTableEntry* tabela_paginas; // Tabela de páginas específica do processo
} Processo;

// Variáveis Globais (definidas após a leitura da config)
int NUM_FRAMES;
int TAMANHO_PAGINA;
int NUM_PROCESSOS;
Processo* PROCESSOS;
Frame* MEMORIA_FISICA;

// Variaveis de controle
int tempo_global = 0; // Para rastrear o tempo global para FIFO
int proximo_frame_livre = 0; // Índice do próximo frame livre para alocação
int memoria_cheia = 0; // Flag para indicar se a memória física está cheia

//Variaveis de Algoritmo
int ponteiro_clock = 0;

//Variaveis de resumo

int num_page_faults = 0;
int num_acessos = 0;

int main(int argc, char *argv[]){
    if(argc != 4){
        fprintf(stderr, "Uso: %s <algoritmo> <arquivo_config> <arquivo_acessos>\n", argv[0]);
        return 1;
    }

    char *algoritmo = argv[1];
    char *arq_config = argv[2];
    char *arq_acessos = argv[3];


    //Verifica de o algoritmo é válido
    if (strcmp(algoritmo, "fifo") != 0 && strcmp(algoritmo, "clock") != 0) {
        fprintf(stderr, "Erro: Algoritmo deve ser 'fifo' ou 'clock'.\n");
        return 1;
    }

    //Le a configuração, aloca a memoria fisica e os processos
    //inicializa as tabelas de páginas para cada processo
    if (!ler_configuracao(arq_config)) {
        fprintf(stderr, "Erro ao carregar o arquivo de configuração.\n");
        return 1;
    }

    // Processa os acessos do arquivo de acessos
    processar_acessos(algoritmo, arq_acessos);

    //Imprime o resumo dos resultados
    liberar_memoria();

    //Limpa a memória
    return 0;
}

int ler_configuracao(char *filename){
    FILE *file_config;
    int i, j;

    file_config = fopen(filename, "r");
    if (file_config == NULL){
        perror("Erro ao abrir o arquivo de configuração");
        return 0;
    }

    if(fscanf(file_config, "%d", &NUM_FRAMES) != 1 || fscanf(file_config, "%d", &TAMANHO_PAGINA) != 1 || fscanf(file_config, "%d", &NUM_PROCESSOS) != 1){
        fprintf(stderr, "Erro ao ler os parametros globais.\n");
        fclose(file_config);
        return 0;
    }

    // Aloca a memória física
    MEMORIA_FISICA = (Frame*) malloc(NUM_FRAMES * sizeof(Frame));
    if (MEMORIA_FISICA == NULL){
        return 0;
    }

    //Aloca o array de processos
    PROCESSOS = (Processo*) malloc(NUM_PROCESSOS * sizeof(Processo));
    if (PROCESSOS == NULL){
        free(MEMORIA_FISICA);
        return 0;
    }

    //Le e inicializa os processos e tabelas
    for(int i = 0; i < NUM_PROCESSOS; i++){
        int pid, tamanho_virtual;
        if(fscanf(file_config, "%d %d", &pid, &tamanho_virtual) != 2){
            fprintf(stderr, "Erro ao ler os parametros do processo %d.\n", i);
            free(file_config);
            return 0;
        }

        PROCESSOS[i].pid = pid;
        PROCESSOS[i].tamanho_virtual = tamanho_virtual;

        //Calcula o número de páginas virtuais
        PROCESSOS[i].num_paginas = tamanho_virtual / TAMANHO_PAGINA;
        PROCESSOS[i].tabela_paginas = (PageTableEntry*) malloc(PROCESSOS[i].num_paginas * sizeof(PageTableEntry));
        if(PROCESSOS[i].tabela_paginas == NULL){
            fprintf(stderr, "Erro ao alocar a tabela de páginas para o processo %d.\n", pid);
            fclose(file_config);
            return 0;
        }

        //Inicializa a tabela de páginas
        for(int j = 0; j < PROCESSOS[i].num_paginas; j++){
            PROCESSOS[i].tabela_paginas[j].frame_id = -1; // Indica que a página não está na memória
            PROCESSOS[i].tabela_paginas[j].valid_bit = 0; // Página não válida
            PROCESSOS[i].tabela_paginas[j].referenced_bit = 0; // R-bit inicializado como 0
            PROCESSOS[i].tabela_paginas[j].tempo_chegada_fifo = 0; // Tempo de chegada para FIFO
        }
    }

    fclose(file_config);
    return 1;
}
