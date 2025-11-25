# Relatório do Projeto 2: Simulador de Memória Virtual

**Disciplina:** Sistemas Operacionais
**Professor:** Lucas Figueiredo
**Data:**

## Integrantes do Grupo

- Matheus Guion - 10437693
- Henrique Ferreira - 10439797
- Enrique Cipolla - 10427834

---

## 1. Instruções de Compilação e Execução

### 1.1 Compilação

Descreva EXATAMENTE como compilar seu projeto. Inclua todos os comandos necessários.

primeiro passo: gcc -o simulador main.c simulador.c memoria.c parser.c
segundo passo: ./src/simulador clock tests/config_X.txt tests/acessos_X.txt 
Obs: o X representa o número desejado para testar os códigos, então apenas substitua pelo valor desejado

### 1.2 Execução

Forneça exemplos completos de como executar o simulador.

**Exemplo com FIFO:**
./src/simulador fifo tests/config_X.txt tests/acessos_X.txt

**Exemplo com Clock:**
./src/simulador clock tests/config_X.txt tests/acessos_X.txt

---

## 2. Decisões de Design

### 2.1 Estruturas de Dados

Descreva as estruturas de dados que você escolheu para representar:

**Tabela de Páginas:**
- Qual estrutura usou? (array, lista, hash map, etc.)
No nosso projeto estruturamos nossa tabela de páginas usando um array

- Quais informações armazena para cada página?
Nossa tabela de páginas foi dividida em: frame_id, valid_bit, referenced_bit e tempo_chegada_fifo 
frame_id -> armazena o índice do frame na memória física onde se encontra o endereço virtual
valid_bit -> bit válido ou inválido, possui valor 1 se encontrar a pagina na memória física ou valor 0 se não encontrar
referenced_bit -> define o nosso R-bit, vai ser 1 caso a página foi referenciada recentemente
tempo_chegada_fifo -> um valor inteiro que registra o tempo em que a página foi trazida a memória

- Como organizou para múltiplos processos?
Para poder realizar múltiplos processos, o simulador usa um array global de estruturas, e cada processo contém o ponteiro que referencia a sua tabela de páginas. O array de processos é uma lista de tamanho fixo armazena todas as instâncias que paticipam da simulação, nossa tabela_paginas é um ponteiro para um array no qual cada processo é alocado dinamicamente no parser e tem seu tamanho definido previamente, e o endereçamento funciona a partir do acesso, a partir do momento que ocorre o acesso na memória virtual o simulador identifica o pid para encontrar o processo dentro do array global PROCESSOS, em seguida usamos pagina_virtual como índice para acessar o array tabela_paginas

- Por que escolheu essa abordagem?
Usar esse tipo de abordagem nos deu diversas vantagens como permitir um acesso rápido à tábela de páginas, uma implementação mais eficiente e permite isolar os processos. Usando o número da página virtual como índice do array, permite que a tradução seja rápida já que essa implementação faz com que o tempo de busca seja constante e imediato, usamos as informações armazenadas dentro de cada página com o intuito de simplificar e agilizar a lógica dos algoritmos de substituição. O isolamento de processos permite que outros processos interfiram na memória virtual de outros processos

**Frames Físicos:**
- Como representou os frames da memória física?
Os frames de memória são representados por um array global alocado dinamicamente

- Quais informações armazena para cada frame?
Armazenamos o pid, pagina_id e livre
pid -> identificador do processo
pagina_id -> número da página virtual que está no frame
livre -> verifica se está ivre ou ocupado usando 1 e 0

- Como rastreia frames livres vs ocupados?
Rastreamos o estado do frame usando a variável livre que é o principal indicador dependendo do valor armazenado na variável livre, depois rastreamos o indice usando proximo_frame_livre para encontrar o primeiro frame que foi encontrado como livre, otimizando a busca, e como verificação final usamos memoria_cheia como uma flag booleana que indica se todos os frames foram ocupados pelo menos uma vez

-Por que escolheu essa abordagem?
A decisão de uma array para memória física é a abordagem mais eficiente, o uso de um índice permite permite acesso direto a qualquer frame. O registro de pid e pagina_id permite fazer o mapeamento reverso para saber qual página virtual está em qual frame físico

**Estrutura para FIFO:**
- Como mantém a ordem de chegada das páginas?
A ordem de chegada é registrada diretamente na entrada da tábela de páginas, o campo tempo_chegada_fifo é atualizada a cada page fault, e a varoávek tempo_global é incrementada a cada acesso à memória

- Como identifica a página mais antiga?
A função executar_fifo itera sobre todos os frames, para cada frame ocupado, ela busca na tabela de páginas o valor de tempo_chegada_fifo, a vítima escolhida é que tiver o menor tempo_chegada_fifo

- Por que escolheu essa abordagem?
Ao invés de usar uma estrutura complexa que exige atualização a cada substituição o uso de registro tempo simplifica. O tempo de chegada como metainformação permite que a busca da vítima seja feita de forma direta

**Estrutura para Clock:**
- Como implementou o ponteiro circular?
O mecanismo circular é implementado por um array e variável global (ponteiro_clock), a variável global armazena o índice do próximo frame a ser inspecionado

- Como armazena e atualiza os R-bits?
O R-bit é armazenado como referenced_bit, sempre que a página em memória é acessada, o referenced_bit é definido como 1, durante a execução do executar_clock se o ponteiro encontra um R-bit em 1, ele é zerado e a página ganha a second chance

-Por que escolheu essa abordagem?
Usar o índice do array como ponteiro circular é simples e barato. O armazenamento do R-bit na Tábela de Páginas permite que o estado de uso da página esteja acessível e seja atualizado a cada acesso

### 2.2 Organização do Código

Descreva como organizou seu código:

- Quantos arquivos/módulos criou?
Usamos seis arquivos

- Qual a responsabilidade de cada arquivo/módulo?
simulador.h -> define estruturas de dados e variáveis globais

simulador.c -> contém a definição das variáveis globais do .h, e implementa funções de liberação de memória e impressão do resumo

parser.h -> declara funções necessárias para ler os arquivos de entrada, agindo como interface entre simulador e sistema de arquivo

memoria.c -> contém o loop principal de simulação e implementação dos algoritms de substituição

main.c -> ponto de entrada do programa, trata argumentos de linha de comando, chama inicialização e incia loop de simulação

- Quais são as principais funções e o que cada uma faz?
main (argc, argv) -> função de coordeação, verifica os argumentos de entrada, chama o parser_carregar_configuracao para iniciar as estruturas e em seguida chama processar_acessos para inciar a simulação, exibe resumo e libera memória

simulador.c:
   - liberar_memoria() -> funções de limpeza. Desaloca toda a memória alocada dinamicamente, incluindo arrays de frames e todas as tabelas de páginas, evitando memory leaks
   - imprimir_resumo(algoritmo) -> exibe métricas finais da simulação

parser.c:
   - parse_carregar_configuracao(filename) -> lê os arquivos de configuração incial, extrai NUM_FRAMES, TAMANHO_PAGINA E NUM_PROCESSOS, aloca e inicializa a MEMORIA_FISICA e tabelas de pagina de cada processo
   - parser_abrir_arquivo_acessos(filename) -> abre o arquivo contendo a sequencia de acessos à memória
   - parser_ler_proximo_acesso(file, pid, endereco) -> lê uma linha do arquivo de acessos, extraindo o pid e endereço virtual preparando os dados para o loop de simulação

memoria.c:
   - processar_acessos(algortmo, filename) -> contém o loop principal que lê cada acesso, a cada iteração incremente tempo_global, calcula pagina virtual, consulta o valid_bit e trata ocorrência de hit ou page fault
   - executar_fifo() -> implementa a lógica de substituição FIFO, itera sobre todos os frames da memória física, busca o menor valor de tempo_chegada_fifo na Tabela de Páginas e retorna o ID do frame vítima
   - executar_clock() -> implementa a lógica do CLOCK. Usa variável ponteiro_clock para percorrer os frames em loop circular, zera o referenced_bit se for 1 e avança ou seleciona a página se for 0 e avança
   - buscar_processo_idx(pid) -> função que pesquisa o índice do processo dentro do array global a partir do seu PID


### 2.3 Algoritmo FIFO

Explique **como** implementou a lógica FIFO:

- Como mantém o controle da ordem de chegada?
- Como seleciona a página vítima?
- Quais passos executa ao substituir uma página?

**Não cole código aqui.** Explique a lógica em linguagem natural.

### 2.4 Algoritmo Clock

Explique **como** implementou a lógica Clock:

- Como gerencia o ponteiro circular?
- Como implementou a "segunda chance"?
- Como trata o caso onde todas as páginas têm R=1?
- Como garante que o R-bit é setado em todo acesso?

**Não cole código aqui.** Explique a lógica em linguagem natural.

### 2.5 Tratamento de Page Fault

Explique como seu código distingue e trata os dois cenários:

**Cenário 1: Frame livre disponível**
- Como identifica que há frame livre?
- Quais passos executa para alocar a página?

**Cenário 2: Memória cheia (substituição)**
- Como identifica que a memória está cheia?
- Como decide qual algoritmo usar (FIFO vs Clock)?
- Quais passos executa para substituir uma página?

---

## 3. Análise Comparativa FIFO vs Clock

### 3.1 Resultados dos Testes

Preencha a tabela abaixo com os resultados de pelo menos 3 testes diferentes:

| Descrição do Teste | Total de Acessos | Page Faults FIFO | Page Faults Clock | Diferença |
|-------------------|------------------|------------------|-------------------|-----------|
| Teste 1 - Básico  |         8         |         5         |        5           |     0      |
| Teste 2 - Memória Pequena |   10       |        10          |         10          |     0      |
| Teste 3 - Simples |       7           |         4         |         4          |      0     |
| Teste Próprio 1   |        20          |        16          |        16           |     0      |

### 3.2 Análise

Com base nos resultados acima, responda:

1. **Qual algoritmo teve melhor desempenho (menos page faults)?**
O teste que fizemos a cima o desempenho foi igual para ambos

2. **Por que você acha que isso aconteceu?** Considere:
   - Como cada algoritmo escolhe a vítima
   FIFO -> escolhe a página presente na memória a mais tempo
   CLOCK -> usa o ponteiro circular para percorrer os frames e o R-bit para decidir se a próxima página merece uma segunda chance

   - O papel do R-bit no Clock
   É o mecanismo de filtragem do CLOCK, quando a página é acessada o R-bit é setado para 1. A segunda chance ocorre quando temos um page fault e o ponteiro encontra R-bit = 1, não a substitui ao invés disso o R-bit é zerado, a página só é escolhida como vítima se o ponteiro encontrar com R-bit = 0

   - O padrão de acesso dos testes
   No teste feito acima o padrão acesso é provável que tenha sido uma swequencia pura,mostrando que ocorrem acessos nunca repetidos

3. **Em que situações Clock é melhor que FIFO?**
O CLOCK é mlhor que o FIFO quando é necessário o uso de localidade temporal. O CLOCK impede a substituição de páginas que estão em uso frequente, o CLOCK usa o R-bit para dar uma segunda chance a essas páginas
   - Dê exemplos de padrões de acesso onde Clock se beneficia
   A, B, C, D, A, E (Memória de 4 frames)

4. **Houve casos onde FIFO e Clock tiveram o mesmo resultado?**
Sim nos quatro testes fietos
   - Por que isso aconteceu?
   Neste caso provavelmente acontece pois o ponteiro do CLOCK aponta para a mesma vítima do FIFO

5. **Qual algoritmo você escolheria para um sistema real e por quê?**
Escolheria o CLOCK, pois mesmo que no exemplo de teste os resultados tenham sidos iguais na maioria dos casos o CLOCK terá um funcionamento superior ao FIFO

---

## 4. Desafios e Aprendizados

### 4.1 Maior Desafio Técnico

Descreva o maior desafio técnico que seu grupo enfrentou durante a implementação:

- Qual foi o problema?
Fazer a implementação do header de forma que as structs estivessem corretamentes correlacionadas com as funções

- Como identificaram o problema?
Fazendo a análise das variáveis e de como elas estavam funcionando e se comumicando

- Como resolveram?
Substituindo as variáveis necessárias pelas corretas e fazer as relações necessárias

- O que aprenderam com isso?
Aprendemos que uma das partes cruciais da construção de uma paginação mora na maneira com a qual é feita a implementação de determinados dados

### 4.2 Principal Aprendizado

Descreva o principal aprendizado sobre gerenciamento de memória que vocês tiveram com este projeto:

- O que vocês não entendiam bem antes e agora entendem?
O funcionamento e conceito de paginação, a maneira com a qual a tabela de páginas conversa com o endereço do frame e o resto das informações

- Como este projeto mudou sua compreensão de memória virtual?
Este projeto nos ajudou a ter um melhor entendimento sobre alguns aspectos essenciais de memória virtual, como a forma com a qual os endereços são lidos pelo sistema e como isso afeta o funcionamento do sistema como um todo

- Que conceito das aulas ficou mais claro após a implementação?
O conceito de método CLOCK de substituição, alguns aspectos particulares dele tornavam complicado o entendimento de como ele poderia ser implementado, mas a fazendo o simulador foi possível entender perfritamente sua implementação

---

## 5. Vídeo de Demonstração

**Link do vídeo:** [Insira aqui o link para YouTube, Google Drive, etc.]

### Conteúdo do vídeo:

Confirme que o vídeo contém:

- [ ] Demonstração da compilação do projeto
- [ ] Execução do simulador com algoritmo FIFO
- [ ] Execução do simulador com algoritmo Clock
- [ ] Explicação da saída produzida
- [ ] Comparação dos resultados FIFO vs Clock
- [ ] Breve explicação de uma decisão de design importante

---

## Checklist de Entrega

Antes de submeter, verifique:

- [ ] Código compila sem erros conforme instruções da seção 1.1
- [ ] Simulador funciona corretamente com FIFO
- [ ] Simulador funciona corretamente com Clock
- [ ] Formato de saída segue EXATAMENTE a especificação do ENUNCIADO.md
- [ ] Testamos com os casos fornecidos em tests/
- [ ] Todas as seções deste relatório foram preenchidas
- [ ] Análise comparativa foi realizada com dados reais
- [ ] Vídeo de demonstração foi gravado e link está funcionando
- [ ] Todos os integrantes participaram e concordam com a submissão

---
## Referências
Liste aqui quaisquer referências que utilizaram para auxiliar na implementação (livros, artigos, sites, **links para conversas com IAs.**)


---

## Comentários Finais

Use este espaço para quaisquer observações adicionais que julguem relevantes (opcional).

---
