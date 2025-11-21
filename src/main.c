#include "simulador.h"

int main(int argc, char *argv[]){
    if(argc != 4){
        fprintf(stderr, "Uso: %s <algoritmo> <arquivo_config> <arquivo_acessos>\n", argv[0]);
        return 1;
    }

    char *algoritmo = argv[1];
    char *arq_config = argv[2];
    char *arq_acessos = argv[3];

    // Verifica se o algoritmo é válido
    if (strcmp(algoritmo, "fifo") != 0 && strcmp(algoritmo, "clock") != 0) {
        fprintf(stderr, "Erro: Algoritmo deve ser 'fifo' ou 'clock'.\n");
        return 1;
    }

    // Le a configuração, aloca a memoria fisica e os processos
    if (!ler_configuracao(arq_config)) {
        // A função ler_configuracao deve liberar a memória alocada em caso de erro.
        fprintf(stderr, "A simulação nao pode continuar.\n");
        return 1;
    }

    // Processa os acessos do arquivo de acessos
    processar_acessos(algoritmo, arq_acessos);

    // Imprime o resumo dos resultados
    imprimir_resumo(algoritmo);

    // Limpa a memória
    liberar_memoria();

    return 0;
}
