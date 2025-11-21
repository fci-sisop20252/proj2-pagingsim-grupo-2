# Conceitos de Memória Virtual e Paginação

Este documento apresenta os conceitos teóricos necessários para implementar o simulador. A responsabilidade de traduzir esses conceitos em código é sua. Não existe uma única forma "correta" de implementar - o importante é compreender a lógica e aplicá-la de forma funcional.

## Introdução

A memória virtual é uma abstração fundamental em sistemas operacionais modernos. Este projeto simula o mecanismo de paginação, permitindo que você compreenda na prática como endereços virtuais são traduzidos para endereços físicos e como o sistema operacional gerencia uma memória física menor que a memória virtual total dos processos.

## 1. Memória Virtual e Paginação

### Conceito de Memória Virtual

Memória virtual é uma técnica que fornece a cada processo uma abstração de memória contínua e isolada, independente da organização física da memória RAM. Com memória virtual:

- Cada processo "enxerga" seu próprio espaço de endereçamento, começando do endereço 0
- Processos são isolados uns dos outros (não podem acessar memória de outros processos)
- A memória física pode ser menor que a soma das memórias virtuais de todos os processos

### Divisão em Páginas e Frames

O espaço de endereçamento virtual é dividido em blocos de tamanho fixo chamados **páginas** (pages). A memória física também é dividida em blocos do mesmo tamanho chamados **frames** (ou page frames).

**Exemplo:**
- Memória virtual de um processo: 50 bytes
- Tamanho da página: 10 bytes
- Número de páginas virtuais: 50 / 10 = 5 páginas (páginas 0, 1, 2, 3, 4)

### Vantagens da Paginação

- **Sem fragmentação externa**: Como páginas têm tamanho fixo, qualquer frame livre pode ser usado
- **Proteção**: Cada processo tem sua própria tabela de páginas
- **Compartilhamento**: Páginas podem ser compartilhadas entre processos (não explorado neste projeto)

## 2. Tradução de Endereços

Quando um processo acessa um endereço virtual, o sistema deve traduzi-lo para o endereço físico correspondente. Este é o trabalho da MMU (Memory Management Unit).

### Processo de Tradução

Um endereço virtual é dividido em duas partes:

1. **Número da página (p)**: Identifica qual página virtual está sendo acessada
2. **Deslocamento (d)**: Posição dentro da página

No nosso simulador, usamos aritmética decimal simples:

```
p = endereco_virtual / tamanho_da_pagina
d = endereco_virtual % tamanho_da_pagina
```

### Exemplo Numérico Completo

Suponha:
- Tamanho da página: 10 bytes
- Endereço virtual: 25

Tradução:
- p = 25 / 10 = 2 (página 2)
- d = 25 % 10 = 5 (deslocamento 5)

Significado: O endereço virtual 25 está na página 2, na posição 5 dentro dessa página.

Para encontrar o endereço físico, consultamos a tabela de páginas:
- Se a página 2 está no frame 7, então:
- Endereço físico = (frame × tamanho_da_pagina) + deslocamento
- Endereço físico = (7 × 10) + 5 = 75

## 3. Tabela de Páginas

Cada processo tem uma **tabela de páginas** que mapeia páginas virtuais para frames físicos.

### Informações na Tabela de Páginas

Para cada página virtual, a tabela armazena:

- **Número do frame**: Em qual frame físico a página está carregada (se estiver)
- **Valid bit (V)**: Indica se a página está na memória física (V=1) ou não (V=0)
- **Referenced bit (R)**: Indica se a página foi recentemente acessada (usado pelo algoritmo Clock)

### Exemplo de Tabela de Páginas

Processo PID 0 com 5 páginas virtuais:

```
Página Virtual | Frame | V | R
      0        |   3   | 1 | 1
      1        |   -   | 0 | -
      2        |   7   | 1 | 0
      3        |   1   | 1 | 1
      4        |   -   | 0 | -
```

Interpretação:
- Página 0 está no frame 3, na memória (V=1), foi recentemente acessada (R=1)
- Página 1 não está na memória (V=0)
- Página 2 está no frame 7, na memória (V=1), não foi recentemente acessada (R=0)

### Tabelas Independentes

Cada processo tem sua própria tabela de páginas. Isso garante isolamento: o processo 0 página 2 é diferente do processo 1 página 2.

## 4. Page Fault

Um **page fault** ocorre quando um processo tenta acessar uma página que não está na memória física (valid bit = 0).

### Como Tratar um Page Fault

Quando ocorre um page fault:

1. **Verificar se há frame livre:**
   - Se sim: Alocar a página no frame livre, atualizar tabela de páginas
   - Se não: Aplicar algoritmo de substituição para escolher uma página vítima

2. **Carregar a página**: Em um sistema real, isso envolveria ler a página do disco (swap). No nosso simulador, apenas alocamos a página no frame.

3. **Atualizar tabela de páginas**: Setar valid bit = 1, armazenar número do frame, setar referenced bit = 1

4. **Continuar execução**: Refazer o acesso (agora será um HIT)

### Custo de Page Faults

Em sistemas reais, page faults são operações caras porque envolvem I/O de disco (milissegundos vs nanossegundos da RAM). Por isso, algoritmos que minimizam page faults melhoram o desempenho.

## 5. Algoritmo FIFO (First In First Out)

### Conceito

O algoritmo FIFO substitui a página que está há **mais tempo** na memória, independentemente de seu padrão de uso.

### Lógica

- Manter uma estrutura (fila, lista, etc.) que registra a ordem de chegada das páginas
- Quando precisar escolher uma vítima, escolher a página que chegou primeiro
- Remover a vítima da estrutura e adicionar a nova página no final

### Vantagens

- **Simplicidade**: Fácil de entender e implementar
- **Determinístico**: Sempre escolhe a página mais antiga

### Desvantagens

- **Não considera padrão de acesso**: Pode remover uma página frequentemente usada
- **Anomalia de Belady**: Em alguns casos, adicionar mais frames pode aumentar page faults

### Exemplo de Funcionamento

Suponha memória com 3 frames, inicialmente vazia:

```
Acessos: P0-0, P0-1, P0-2, P0-3, P0-0

Estado da memória (ordem FIFO: da esquerda para direita):
Acesso P0-0: [P0-0]               (PAGE FAULT - frame livre)
Acesso P0-1: [P0-0, P0-1]         (PAGE FAULT - frame livre)
Acesso P0-2: [P0-0, P0-1, P0-2]   (PAGE FAULT - frame livre)
Acesso P0-3: [P0-1, P0-2, P0-3]   (PAGE FAULT - remove P0-0, mais antiga)
Acesso P0-0: [P0-2, P0-3, P0-0]   (PAGE FAULT - remove P0-1, mais antiga)
```

## 6. Algoritmo Clock (Segunda Chance)

### Conceito

O algoritmo Clock é uma melhoria do FIFO que leva em consideração se a página foi **recentemente acessada**, usando o referenced bit (R-bit).

### Lógica

- Organizar os frames em estrutura circular (relógio)
- Manter um ponteiro que percorre os frames circularmente
- Quando precisar escolher uma vítima:
  1. Examinar página apontada pelo ponteiro
  2. Se R=1: Dar "segunda chance" - zerar R-bit e avançar ponteiro
  3. Se R=0: Escolher como vítima
  4. Repetir até encontrar vítima

### Por que "Segunda Chance"?

Se uma página tem R=1, significa que foi recentemente acessada. O algoritmo zera o R-bit mas não remove a página imediatamente (dá uma "segunda chance"). Se a página for acessada novamente antes do ponteiro completar a volta, o R-bit será setado novamente e ela receberá outra chance.

### Vantagens

- **Considera padrão de acesso**: Páginas recentemente usadas têm menor chance de serem removidas
- **Melhor que FIFO**: Na maioria dos casos, resulta em menos page faults
- **Aproximação do LRU**: Mais simples que LRU (Least Recently Used) mas com desempenho similar

### Desvantagens

- **Mais complexo que FIFO**: Requer gerenciar ponteiro circular e R-bits
- **Pode percorrer todos os frames**: No pior caso (todos R=1), o ponteiro dará uma volta completa

### Exemplo de Funcionamento

Suponha memória com 3 frames, ponteiro inicialmente no frame 0:

```
Acessos: P0-0, P0-1, P0-2, P0-3, P0-0

Estado: [Frame: Página (R-bit)] - Ponteiro: ^

Acesso P0-0: [0: P0-0 (1), -, -]                    ^  (PAGE FAULT - frame livre, R=1)
Acesso P0-1: [0: P0-0 (1), 1: P0-1 (1), -]          ^  (PAGE FAULT - frame livre, R=1)
Acesso P0-2: [0: P0-0 (1), 1: P0-1 (1), 2: P0-2 (1)]^  (PAGE FAULT - frame livre, R=1)

Acesso P0-3: (PAGE FAULT - memória cheia)
  Ponteiro em frame 0: P0-0 (R=1) -> zera R, avança
  Ponteiro em frame 1: P0-1 (R=1) -> zera R, avança
  Ponteiro em frame 2: P0-2 (R=1) -> zera R, avança (volta ao início)
  Ponteiro em frame 0: P0-0 (R=0) -> VÍTIMA!
  Estado: [0: P0-3 (1), 1: P0-1 (0), 2: P0-2 (0)]   ^

Acesso P0-0: (PAGE FAULT - memória cheia)
  Ponteiro em frame 1: P0-1 (R=0) -> VÍTIMA!
  Estado: [0: P0-3 (1), 1: P0-0 (1), 2: P0-2 (0)]      ^
```

## 7. Referenced Bit (R-bit)

### Hardware Real

Em hardware real, a MMU seta automaticamente o R-bit quando a página é acessada (leitura ou escrita). O sistema operacional pode ler e zerar o R-bit, mas não precisa intervir em cada acesso.

### No Nosso Simulador

Como estamos simulando, **você** é responsável por setar o R-bit:

- **Em TODO acesso**: Setar R=1, seja em HIT ou após carregar a página em PAGE FAULT
- **No algoritmo Clock**: Zerar R-bit ao dar segunda chance

### Importante

Não esquecer de setar R=1 após carregar uma página em PAGE FAULT! A página acabou de ser acessada, então seu R-bit deve ser 1.

## 8. Exemplo Passo-a-Passo Completo

Vamos simular alguns acessos manualmente para consolidar os conceitos.

### Configuração

- Frames: 3
- Tamanho da página: 10
- Processos: 1 (PID 0, tamanho 40 - 4 páginas: 0, 1, 2, 3)
- Acessos: 5, 15, 25, 35, 5

### Estado Inicial

Memória física: [ - , - , - ] (3 frames vazios)
Tabela de páginas P0: Todas páginas com V=0

### Acesso 1: PID 0, Endereço 5

- Tradução: p=5/10=0, d=5%10=5 → Página 0, Deslocamento 5
- Consulta tabela: Página 0 não está na memória (V=0)
- **PAGE FAULT**
- Frame livre disponível (frame 0)
- Carregar página 0 no frame 0
- Atualizar tabela: Página 0 → Frame 0, V=1, R=1
- Memória: [ P0-0(R=1) , - , - ]

### Acesso 2: PID 0, Endereço 15

- Tradução: p=15/10=1, d=15%10=5 → Página 1, Deslocamento 5
- Consulta tabela: Página 1 não está na memória (V=0)
- **PAGE FAULT**
- Frame livre disponível (frame 1)
- Carregar página 1 no frame 1
- Atualizar tabela: Página 1 → Frame 1, V=1, R=1
- Memória: [ P0-0(R=1) , P0-1(R=1) , - ]

### Acesso 3: PID 0, Endereço 25

- Tradução: p=25/10=2, d=25%10=5 → Página 2, Deslocamento 5
- Consulta tabela: Página 2 não está na memória (V=0)
- **PAGE FAULT**
- Frame livre disponível (frame 2)
- Carregar página 2 no frame 2
- Atualizar tabela: Página 2 → Frame 2, V=1, R=1
- Memória: [ P0-0(R=1) , P0-1(R=1) , P0-2(R=1) ]

### Acesso 4: PID 0, Endereço 35

- Tradução: p=35/10=3, d=35%10=5 → Página 3, Deslocamento 5
- Consulta tabela: Página 3 não está na memória (V=0)
- **PAGE FAULT**
- Não há frame livre (memória cheia)
- Aplicar algoritmo de substituição:

**FIFO:**
- Vítima: Página 0 (primeira a chegar)
- Remover página 0 do frame 0
- Carregar página 3 no frame 0
- Memória: [ P0-3(R=1) , P0-1(R=1) , P0-2(R=1) ]

**Clock (ponteiro em 0):**
- Frame 0: P0-0 (R=1) → Segunda chance, R=0, avança
- Frame 1: P0-1 (R=1) → Segunda chance, R=0, avança
- Frame 2: P0-2 (R=1) → Segunda chance, R=0, avança (volta)
- Frame 0: P0-0 (R=0) → Vítima!
- Remover página 0 do frame 0
- Carregar página 3 no frame 0
- Memória: [ P0-3(R=1) , P0-1(R=0) , P0-2(R=0) ]
- Ponteiro avança para frame 1

### Acesso 5: PID 0, Endereço 5

- Tradução: p=5/10=0, d=5%10=5 → Página 0, Deslocamento 5
- Consulta tabela: Página 0 não está na memória (foi removida)
- **PAGE FAULT**
- Não há frame livre (memória cheia)
- Aplicar algoritmo de substituição:

**FIFO:**
- Vítima: Página 1 (primeira após remover página 0)
- Remover página 1 do frame 1
- Carregar página 0 no frame 1
- Memória: [ P0-3(R=1) , P0-0(R=1) , P0-2(R=1) ]

**Clock (ponteiro em 1):**
- Frame 1: P0-1 (R=0) → Vítima!
- Remover página 1 do frame 1
- Carregar página 0 no frame 1
- Memória: [ P0-3(R=1) , P0-0(R=1) , P0-2(R=0) ]
- Ponteiro avança para frame 2

### Resumo

Neste exemplo, FIFO e Clock fizeram as mesmas escolhas, mas note que Clock mantém informação sobre R-bits. Em sequências de acesso mais complexas, Clock tende a ter menos page faults.

## Conclusão

Os conceitos apresentados aqui são a base teórica para implementar o simulador. Agora é sua vez de:

1. Decidir quais estruturas de dados usar (arrays, listas, structs, classes, etc.)
2. Implementar a lógica de tradução de endereços
3. Implementar a detecção de HIT vs PAGE FAULT
4. Implementar os algoritmos FIFO e Clock
5. Testar e comparar os resultados

Boa sorte!
