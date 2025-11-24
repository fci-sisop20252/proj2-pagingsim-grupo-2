#include "simulador.h"
#include "parser.h" // Incluindo o parser

// Protótipos locais
int executar_fifo();
int executar_clock();
int buscar_processo_idx(int pid);

int buscar_processo_idx(int pid) {
    for (int i = 0; i < NUM_PROCESSOS; i++) {
        if (PROCESSOS[i].pid == pid) return i;
    }
    return -1;
}

void processar_acessos(char *algoritmo, char *filename) {
    // 1. Abre o arquivo usando o Parser
    FILE *arquivo_acessos = parser_abrir_arquivo_acessos(filename);
    if (!arquivo_acessos) return;

    int pid, endereco_virtual;
    
    // 2. Loop usando o Parser: Enquanto houver linhas para ler
    while (parser_ler_proximo_acesso(arquivo_acessos, &pid, &endereco_virtual)) {
        
        num_acessos++;
        tempo_global++; 

        // 1. Tradução
        int pagina_virtual = endereco_virtual / TAMANHO_PAGINA;
        int deslocamento = endereco_virtual % TAMANHO_PAGINA;

        // 2. Encontrar Processo
        int proc_idx = buscar_processo_idx(pid);
        if (proc_idx == -1) {
            fprintf(stderr, "PID %d não encontrado.\n", pid);
            continue;
        }
        
        PageTableEntry *tabela = PROCESSOS[proc_idx].tabela_paginas;

        // 3. Verificar HIT ou PAGE FAULT
        if (tabela[pagina_virtual].valid_bit == 1) {
            // --- HIT ---
            int frame = tabela[pagina_virtual].frame_id;
            tabela[pagina_virtual].referenced_bit = 1; // R-bit = 1 no acesso

            printf("Acesso: PID %d, Endereço %d (Página %d, Deslocamento %d) -> HIT: Página %d (PID %d) já está no Frame %d\n",
                   pid, endereco_virtual, pagina_virtual, deslocamento, pagina_virtual, pid, frame);
        } else {
            // --- PAGE FAULT ---
            num_page_faults++;
            int frame_alvo = -1;
            
            // Verifica se tem espaço livre na memória física
            int frame_livre_idx = -1;
            for(int i=0; i<NUM_FRAMES; i++){
                if(MEMORIA_FISICA[i].livre == 1) {
                     frame_livre_idx = i;
                     break;
                }
            }

            if (frame_livre_idx != -1) {
                // --- CASO 2a: Tem Frame Livre ---
                frame_alvo = frame_livre_idx;
                
                printf("Acesso: PID %d, Endereço %d (Página %d, Deslocamento %d) -> PAGE FAULT -> Página %d (PID %d) alocada no Frame livre %d\n",
                       pid, endereco_virtual, pagina_virtual, deslocamento, pagina_virtual, pid, frame_alvo);
            } else {
                // --- CASO 2b: Memória Cheia (Substituição) ---
                if (strcmp(algoritmo, "fifo") == 0) {
                    frame_alvo = executar_fifo();
                } else {
                    frame_alvo = executar_clock();
                }

                // Quem estava lá antes?
                int pid_antigo = MEMORIA_FISICA[frame_alvo].pid;
                int pag_antiga = MEMORIA_FISICA[frame_alvo].pagina_id;
                int idx_antigo = buscar_processo_idx(pid_antigo);

                // "Desalocar" a página antiga
                if(idx_antigo != -1) {
                    PROCESSOS[idx_antigo].tabela_paginas[pag_antiga].valid_bit = 0;
                    PROCESSOS[idx_antigo].tabela_paginas[pag_antiga].frame_id = -1;
                    PROCESSOS[idx_antigo].tabela_paginas[pag_antiga].referenced_bit = 0;
                }

                printf("Acesso: PID %d, Endereço %d (Página %d, Deslocamento %d) -> PAGE FAULT -> Memória cheia. Página %d (PID %d) (Frame %d) será desalocada. -> Página %d (PID %d) alocada no Frame %d\n",
                       pid, endereco_virtual, pagina_virtual, deslocamento, pag_antiga, pid_antigo, frame_alvo, pagina_virtual, pid, frame_alvo);
            }

            // --- ATUALIZAR TABELAS E FRAME ---
            MEMORIA_FISICA[frame_alvo].pid = pid;
            MEMORIA_FISICA[frame_alvo].pagina_id = pagina_virtual;
            MEMORIA_FISICA[frame_alvo].livre = 0; 

            tabela[pagina_virtual].valid_bit = 1;
            tabela[pagina_virtual].frame_id = frame_alvo;
            tabela[pagina_virtual].referenced_bit = 1; 
            tabela[pagina_virtual].tempo_chegada_fifo = tempo_global;
        }
    }
    
    // 3. Fecha usando o Parser
    parser_fechar_arquivo_acessos(arquivo_acessos);
}

// --- ALGORITMOS (Copiados do seu arquivo original) ---

int executar_fifo() {
    int frame_vitima = -1;
    int menor_tempo = 2147483647; // MAX INT

    for (int i = 0; i < NUM_FRAMES; i++) {
        int pid = MEMORIA_FISICA[i].pid;
        int pag = MEMORIA_FISICA[i].pagina_id;
        int proc_idx = buscar_processo_idx(pid);

        if (proc_idx == -1) continue; 

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
        int pid = MEMORIA_FISICA[ponteiro_clock].pid;
        int pag = MEMORIA_FISICA[ponteiro_clock].pagina_id;
        int proc_idx = buscar_processo_idx(pid);
        
        if (proc_idx != -1) {
            PageTableEntry *entry = &PROCESSOS[proc_idx].tabela_paginas[pag];

            if (entry->referenced_bit == 1) {
                entry->referenced_bit = 0; // Segunda chance
                ponteiro_clock = (ponteiro_clock + 1) % NUM_FRAMES;
            } else {
                int vitima = ponteiro_clock;
                ponteiro_clock = (ponteiro_clock + 1) % NUM_FRAMES;
                return vitima;
            }
        } else {
             // Caso de borda defensivo se frame estiver "sujo"
             ponteiro_clock = (ponteiro_clock + 1) % NUM_FRAMES;
        }
    }
}