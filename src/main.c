#include "simulador.h"
#include "parser.h"

int main(int argc, char *argv[]){
    if(argc != 4){
        fprintf(stderr, "Uso: %s <algoritmo> <arquivo_config> <arquivo_acessos>\n", argv[0]);
        return 1;
    }

    char *algoritmo = argv[1];
    char *arq_config = argv[2];
    char *arq_acessos = argv[3];

    if (strcmp(algoritmo, "fifo") != 0 && strcmp(algoritmo, "clock") != 0) {
        fprintf(stderr, "Erro: Algoritmo deve ser 'fifo' ou 'clock'.\n");
        return 1;
    }

    // Carrega configuração usando o Parser
    if (!parser_carregar_configuracao(arq_config)) {
        fprintf(stderr, "A simulação nao pode continuar.\n");
        return 1;
    }

    processar_acessos(algoritmo, arq_acessos);

    imprimir_resumo(algoritmo);
    liberar_memoria();

    return 0;
}