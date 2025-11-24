#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "simulador.h"

// Carrega a configuração inicial (frames, tamanho página, processos)
int parser_carregar_configuracao(const char *filename);

// Abre o arquivo de acessos para leitura
FILE* parser_abrir_arquivo_acessos(const char *filename);

// Lê a próxima linha de acesso (retorna 1 se leu, 0 se acabou)
int parser_ler_proximo_acesso(FILE *file, int *pid, int *endereco);

// Fecha o arquivo de acessos
void parser_fechar_arquivo_acessos(FILE *file);

#endif