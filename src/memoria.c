// --- PROTÓTIPOS (Coloque antes do main) ---
#include "simulador.c"

void processar_acessos(char *algoritmo, char *filename);
int executar_fifo();
int executar_clock();
void liberar_memoria();
int buscar_processo_idx(int pid); // Helper

// --- IMPLEMENTAÇÃO DA LÓGICA DE MEMÓRIA ---

// Helper para achar o índice do processo no array PROCESSOS
int buscar_processo_idx(int pid) {
    for (int i = 0; i < NUM_PROCESSOS; i++) {
        if (PROCESSOS[i].pid == pid) return i;
    }
    return -1;
}

void processar_acessos(char *algoritmo, char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo de acessos");
        return;
    }

    int pid, endereco_virtual;
    
    // Loop de leitura linha a linha
    while (fscanf(file, "%d %d", &pid, &endereco_virtual) == 2) {
        num_acessos++;
        tempo_global++; // Relógio lógico avança

        // 1. Tradução (Aritmética Decimal)
        int pagina_virtual = endereco_virtual / TAMANHO_PAGINA;
        int deslocamento = endereco_virtual % TAMANHO_PAGINA;

        // 2. Encontrar Processo
        int proc_idx = buscar_processo_idx(pid);
        if (proc_idx == -1) {
            fprintf(stderr, "PID %d não encontrado.\n", pid);
            continue;
        }
        
        // Ponteiro para a tabela deste processo específico
        PageTableEntry *tabela = PROCESSOS[proc_idx].tabela_paginas;

        // 3. Verificar HIT ou PAGE FAULT
        if (tabela[pagina_virtual].valid_bit == 1) {
            // --- HIT ---
            int frame = tabela[pagina_virtual].frame_id;
            
            // Atualiza R-bit (Necessário para Clock)
            tabela[pagina_virtual].referenced_bit = 1;

            printf("Acesso: PID %d, Endereço %d (Página %d, Deslocamento %d) -> HIT: Página %d (PID %d) já está no Frame %d\n",
                   pid, endereco_virtual, pagina_virtual, deslocamento, pagina_virtual, pid, frame);
        } else {
            // --- PAGE FAULT ---
            num_page_faults++;
            int frame_alvo = -1;
            int substituiu = 0;

            // Verifica se tem espaço livre na memória física
            // Vamos contar quantos frames ocupados existem
            int frames_ocupados = 0;
            int frame_livre_idx = -1;
            
            for(int i=0; i<NUM_FRAMES; i++){
                if(MEMORIA_FISICA[i].livre == 1 || MEMORIA_FISICA[i].pid == -1){
                     // Assumindo que você inicializa MEMORIA_FISICA com pid=-1 ou livre=1
                     // Vou considerar que pid=-1 indica livre baseado no seu struct Frame
                     if(MEMORIA_FISICA[i].pid == -1) {
                         frame_livre_idx = i;
                         break;
                     }
                }
            }

            if (frame_livre_idx != -1) {
                // --- CASO 2a: Tem Frame Livre ---
                frame_alvo = frame_livre_idx;
                
                printf("Acesso: PID %d, Endereço %d (Página %d, Deslocamento %d) -> PAGE FAULT -> Página %d (PID %d) alocada no Frame livre %d\n",
                       pid, endereco_virtual, pagina_virtual, deslocamento, pagina_virtual, pid, frame_alvo);
            } else {
                // --- CASO 2b: Memória Cheia (Substituição) ---
                substituiu = 1;

                if (strcmp(algoritmo, "fifo") == 0) {
                    frame_alvo = executar_fifo();
                } else {
                    frame_alvo = executar_clock();
                }

                // Identificar quem estava lá antes para remover
                int pid_antigo = MEMORIA_FISICA[frame_alvo].pid;
                int pag_antiga = MEMORIA_FISICA[frame_alvo].pagina_id;
                int idx_antigo = buscar_processo_idx(pid_antigo);

                // "Desalocar" a página antiga (Setar valid=0 na tabela dela)
                if(idx_antigo != -1) {
                    PROCESSOS[idx_antigo].tabela_paginas[pag_antiga].valid_bit = 0;
                    PROCESSOS[idx_antigo].tabela_paginas[pag_antiga].frame_id = -1;
                    PROCESSOS[idx_antigo].tabela_paginas[pag_antiga].referenced_bit = 0;
                }

                printf("Acesso: PID %d, Endereço %d (Página %d, Deslocamento %d) -> PAGE FAULT -> Memória cheia. Página %d (PID %d) (Frame %d) será desalocada. -> Página %d (PID %d) alocada no Frame %d\n",
                       pid, endereco_virtual, pagina_virtual, deslocamento, pag_antiga, pid_antigo, frame_alvo, pagina_virtual, pid, frame_alvo);
            }

            // --- ATUALIZAR TABELAS E FRAME (COMUM A 2a e 2b) ---
            
            // 1. Atualizar Frame Físico
            MEMORIA_FISICA[frame_alvo].pid = pid;
            MEMORIA_FISICA[frame_alvo].pagina_id = pagina_virtual;
            MEMORIA_FISICA[frame_alvo].livre = 0; 

            // 2. Atualizar Tabela de Páginas do Processo Atual
            tabela[pagina_virtual].valid_bit = 1;
            tabela[pagina_virtual].frame_id = frame_alvo;
            tabela[pagina_virtual].referenced_bit = 1; // Importante: R=1 ao carregar
            tabela[pagina_virtual].tempo_chegada_fifo = tempo_global; // Marca hora de chegada
        }
    }
    
    // Resumo final exigido
    printf("\n--- Simulação Finalizada (Algoritmo: %s)\n", algoritmo);
    printf("Total de Acessos: %d\n", num_acessos);
    printf("Total de Page Faults: %d\n", num_page_faults);
    
    fclose(file);
}

// --- ALGORITMOS ---

int executar_fifo() {
    int frame_vitima = -1;
    int menor_tempo = 2147483647; // MAX INT

    // Varre a memória física para ver quem tem o menor timestamp
    for (int i = 0; i < NUM_FRAMES; i++) {
        int pid = MEMORIA_FISICA[i].pid;
        int pag = MEMORIA_FISICA[i].pagina_id;
        int proc_idx = buscar_processo_idx(pid);

        // Olha na tabela do dono da página quando ela chegou
        int tempo = PROCESSOS[proc_idx].tabela_paginas[pag].tempo_chegada_fifo;

        if (tempo < menor_tempo) {
            menor_tempo = tempo;
            frame_vitima = i;
        }
    }
    return frame_vitima;
}

int executar_clock() {
    while (1) {
        // Olha para o frame apontado pelo ponteiro
        int pid = MEMORIA_FISICA[ponteiro_clock].pid;
        int pag = MEMORIA_FISICA[ponteiro_clock].pagina_id;
        int proc_idx = buscar_processo_idx(pid);
        
        PageTableEntry *entry = &PROCESSOS[proc_idx].tabela_paginas[pag];

        if (entry->referenced_bit == 1) {
            // Segunda chance: Zera R e avança
            entry->referenced_bit = 0;
            ponteiro_clock = (ponteiro_clock + 1) % NUM_FRAMES;
        } else {
            // Encontrou a vítima (R=0)
            int vitima = ponteiro_clock;
            // Avança o ponteiro para a próxima vez
            ponteiro_clock = (ponteiro_clock + 1) % NUM_FRAMES;
            return vitima;
        }
    }
}

void liberar_memoria() {
    if (MEMORIA_FISICA) free(MEMORIA_FISICA);
    if (PROCESSOS) {
        for (int i = 0; i < NUM_PROCESSOS; i++) {
            if (PROCESSOS[i].tabela_paginas) free(PROCESSOS[i].tabela_paginas);
        }
        free(PROCESSOS);
    }
}