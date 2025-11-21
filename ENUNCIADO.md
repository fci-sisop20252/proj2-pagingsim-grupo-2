# Enunciado do Projeto 2: Simulador de Memória Virtual

## 1. Objetivo

Implementar em **linguagem C** um simulador de gerenciamento de memória virtual baseado em paginação que processe uma sequência de acessos à memória de múltiplos processos e aplique algoritmos de substituição de páginas (FIFO e Clock) quando necessário.

## 2. Simplificações

Para tornar o projeto mais didático e gerenciável, faremos algumas simplificações em relação a uma implementação real de um sistema operacional:

- **Aritmética decimal**: Usaremos operações aritméticas simples (divisão e módulo) ao invés de manipulação de bits para traduzir endereços virtuais
- **Memória física limitada**: Simularemos cenários com memória física menor que o total de páginas virtuais dos processos, forçando page faults e substituição de páginas
- **Processos estáticos**: Os processos e seus tamanhos são definidos no arquivo de configuração e não mudam durante a simulação

## 3. Especificações Técnicas

### 3.1 Linha de Comando

Seu simulador deve ser executado com a seguinte sintaxe:

```
./simulador <algoritmo> <arquivo_config> <arquivo_acessos>
```

Onde:
- `<algoritmo>` é `fifo` ou `clock`
- `<arquivo_config>` contém a configuração da memória e dos processos
- `<arquivo_acessos>` contém a sequência de acessos à memória

### 3.2 Arquivo de Configuração

O arquivo de configuração tem o seguinte formato (uma informação por linha):

```
<numero_de_frames>
<tamanho_da_pagina>
<numero_de_processos>
<PID1> <tamanho_memoria_virtual1>
<PID2> <tamanho_memoria_virtual2>
...
```

**Exemplo:**

```
8
10
2
0 50
1 60
```

Significado:
- Tamanho Total da RAM: 8 frames de memória física disponíveis
- Tamanho de Página: Cada página/frame tem tamanho 10 (endereços 0-9 em uma página)
- 2 processos
- Processo PID 0 com memória virtual de tamanho 50 (precisa de 5 páginas: 0-4)
- Processo PID 1 com memória virtual de tamanho 60 (precisa de 6 páginas: 0-5)

### 3.3 Arquivo de Acessos

O arquivo de acessos contém uma sequência de acessos à memória, um por linha:

```
<PID> <endereco_virtual>
<PID> <endereco_virtual>
...
```

**Exemplo:**

```
0 25
1 12
0 28
1 55
0 42
0 25
1 12
0 5
```

Cada linha representa um acesso do processo `<PID>` ao endereço virtual `<endereco_virtual>`.

### 3.4 Lógica do Simulador

Para cada acesso à memória, seu simulador deve:

1. **Traduzir o endereço virtual** em página e deslocamento:
   - `pagina = endereco / tamanho_da_pagina`
   - `deslocamento = endereco % tamanho_da_pagina`

2. **Consultar a tabela de páginas** do processo:
   - **Cenário 1 (HIT):** Se a página já está na memória física:
     - Setar o R-bit (referenced bit) da página para 1
     - Registrar HIT

   - **Cenário 2 (PAGE FAULT):** Se a página NÃO está na memória física:

     - **Cenário 2a (Frame livre disponível):**
       - Alocar a página em um frame livre
       - Atualizar tabela de páginas
       - Setar R-bit para 1
       - Registrar PAGE FAULT com alocação em frame livre

     - **Cenário 2b (Memória cheia):**
       - Aplicar algoritmo de substituição (FIFO ou Clock)
       - Selecionar página vítima
       - Desalocar página vítima
       - Alocar nova página no frame liberado
       - Atualizar tabelas de páginas
       - Setar R-bit da nova página para 1
       - Registrar PAGE FAULT com substituição

**IMPORTANTE sobre o R-bit:** O referenced bit deve ser setado para 1 em **TODO** acesso à página, seja em um HIT ou após carregar a página em um PAGE FAULT. No algoritmo Clock, o simulador zera o R-bit ao dar "segunda chance" e escolhe como vítima a primeira página com R-bit = 0.

### 3.5 Formato de Saída

Seu simulador deve produzir saída EXATAMENTE no formato especificado abaixo para permitir testes automatizados.

#### Formato para HIT:

```
Acesso: PID <pid>, Endereço <addr> (Página <p>, Deslocamento <d>) -> HIT: Página <p> (PID <pid>) já está no Frame <f>
```

**Exemplo:**
```
Acesso: PID 0, Endereço 25 (Página 2, Deslocamento 5) -> HIT: Página 2 (PID 0) já está no Frame 2
```

#### Formato para PAGE FAULT com frame livre:

```
Acesso: PID <pid>, Endereço <addr> (Página <p>, Deslocamento <d>) -> PAGE FAULT -> Página <p> (PID <pid>) alocada no Frame livre <f>
```

**Exemplo:**
```
Acesso: PID 0, Endereço 25 (Página 2, Deslocamento 5) -> PAGE FAULT -> Página 2 (PID 0) alocada no Frame livre 2
```

#### Formato para PAGE FAULT com memória cheia (substituição):

```
Acesso: PID <pid>, Endereço <addr> (Página <p>, Deslocamento <d>) -> PAGE FAULT -> Memória cheia. Página <p_antiga> (PID <pid_antigo>) (Frame <f>) será desalocada. -> Página <p> (PID <pid>) alocada no Frame <f>
```

**Exemplo:**
```
Acesso: PID 1, Endereço 55 (Página 5, Deslocamento 5) -> PAGE FAULT -> Memória cheia. Página 1 (PID 0) (Frame 0) será desalocada. -> Página 5 (PID 1) alocada no Frame 0
```

#### Resumo final:

Ao final da simulação, imprimir:

```
--- Simulação Finalizada (Algoritmo: <fifo|clock>)
Total de Acessos: <N>
Total de Page Faults: <M>
```

**Exemplo:**
```
--- Simulação Finalizada (Algoritmo: fifo)
Total de Acessos: 8
Total de Page Faults: 5
```

## 4. Entregáveis

1. **Código fonte** - Organize da forma que preferir (arquivo único, múltiplos módulos, etc.)
2. **Relatório em PDF** - Use o RELATORIO_TEMPLATE.md como base
3. **Vídeo de demonstração** - Inclua o link no relatório

## 5. Rubrica de Avaliação

**Total: 10 pontos**

### Funcionalidade (6 pontos)
- Leitura correta dos arquivos de configuração e acessos
- Tradução correta de endereços virtuais (página e deslocamento)
- Implementação correta de HITs
- Implementação correta de PAGE FAULTs com frames livres
- Implementação correta do algoritmo FIFO
- Implementação correta do algoritmo Clock
- Formato de saída conforme especificação

### Relatório (2 pontos)
- Instruções de compilação/execução claras
- Explicação das decisões de design (estruturas de dados)
- Análise comparativa FIFO vs Clock
- Desafios e aprendizados

### Vídeo (2 ponto)
- Demonstração da compilação e execução
- Explicação das decisões de design, saída e funcionamento

## 6. Observações Importantes

1. **R-bit**: O referenced bit deve ser setado para 1 em **TODO** acesso à página, seja em HIT ou após carregar em PAGE FAULT. Isso é fundamental para o correto funcionamento do algoritmo Clock.

2. **Formato de saída**: A saída deve ser **EXATAMENTE** como especificado. Testes automatizados usarão comparação exata (diff). Preste atenção a:
   - Espaços entre palavras
   - Pontuação (vírgulas, dois-pontos, etc.)
   - Quebras de linha
   - Capitalização

3. **Linguagem e organização**: O projeto deve ser implementado em **linguagem C**. Você tem liberdade para escolher:
   - Estruturas de dados (structs, arrays, listas)
   - Organização do código (arquivo único, múltiplos .c/.h)
   - Sistema de compilação (Makefile, gcc direto, script build.sh)

4. **Generalidade**: Seu simulador deve funcionar para **qualquer** entrada válida, não apenas os exemplos fornecidos. Os testes de avaliação podem incluir casos não disponibilizados previamente.

5. **Comparação dos algoritmos**: Parte importante do aprendizado é comparar FIFO e Clock. Em alguns casos, Clock terá menos page faults devido ao uso do R-bit. Analise isso no relatório.

6. **Dúvidas**: Consulte o professor por email ou durante as aulas. Não deixe dúvidas acumularem!
