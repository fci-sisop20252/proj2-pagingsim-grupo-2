#include "parser.h"

int parser_carregar_configuracao(const char *filename){
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
            // Aqui idealmente chamaria liberar_memoria(), mas estamos simplificando
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

FILE* parser_abrir_arquivo_acessos(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo de acessos");
        return NULL;
    }
    return file;
}

int parser_ler_proximo_acesso(FILE *file, int *pid, int *endereco) {
    // Retorna 1 se leu com sucesso, 0 caso contrário
    if (fscanf(file, "%d %d", pid, endereco) == 2) {
        return 1;
    }
    return 0;
}

void parser_fechar_arquivo_acessos(FILE *file) {
    if (file != NULL) {
        fclose(file);
    }
}