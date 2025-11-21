# Perguntas Frequentes (FAQ)

## Geral

### Q: Como devo organizar meu código?

**R:** Você tem total liberdade! Pode usar um único arquivo ou múltiplos módulos. Pode criar diretórios `src/`, `include/`, ou qualquer estrutura que faça sentido para você. A única restrição é que os testes em `tests/` devem permanecer como estão.

### Q: Preciso usar Makefile?

**R:** Não é obrigatório. Você pode usar Makefile, criar um script `build.sh`, ou simplesmente ter um comando de compilação direto (ex: `gcc -o simulador simulador.c`). Inclua as instruções no relatório.

## Implementação

### Q: Como devo representar a tabela de páginas?

**R:** Isso é uma decisão de design sua! Algumas opções:
- Array/vetor indexado por número de página
- Lista de structs/objetos
- Hash map/dicionário

Pense em: cada processo tem sua própria tabela? Como armazenar frame, valid bit e referenced bit?

### Q: Como implementar a fila do FIFO?

**R:** Existem várias formas:
- Lista/array que mantém ordem de chegada
- Fila (queue) com push/pop
- Array circular com índice

O importante é conseguir identificar qual página chegou primeiro quando precisar remover.

### Q: Como funciona o ponteiro circular do Clock?

**R:** O ponteiro percorre os frames em ordem circular (0, 1, 2, ..., N-1, 0, 1, ...). Quando encontra uma página com R=1, zera o R-bit e avança. Quando encontra R=0, escolhe como vítima. Você pode usar:
- Variável que armazena posição atual
- Incrementar e usar módulo (%) para voltar ao início

### Q: Quando devo setar o R-bit?

**R:** O R-bit deve ser setado para 1 em **TODO** acesso à página:
1. Quando há HIT (página já está na memória) → setar R=1
2. Quando há PAGE FAULT e página é carregada → setar R=1 após carregar

O algoritmo Clock zera R-bit ao dar "segunda chance".

### Q: Como distinguir frames livres de ocupados?

**R:** Algumas abordagens:
- Lista/array de frames livres que vai diminuindo
- Contador de frames usados
- Marcar cada frame como livre/ocupado (booleano)

## Entrada e Saída

### Q: O formato de saída precisa ser EXATAMENTE igual?

**R:** **Sim!** Os testes automatizados usam comparação exata. Cada espaço, vírgula, dois-pontos, quebra de linha importa. Use os arquivos `expected_*.txt` como referência e teste com `diff`.

### Q: E se minha saída estiver quase certa mas com pequenas diferenças?

**R:** "Quase" não funciona com testes automatizados. Use `diff` para encontrar as diferenças:
```bash
./simulador fifo tests/config_1.txt tests/acessos_1.txt > minha_saida.txt
diff minha_saida.txt tests/expected_fifo_1.txt
```

Se `diff` mostrar diferenças, ajuste seu código para seguir o formato exato.

### Q: Posso adicionar prints de debug?

**R:** Durante desenvolvimento, sim! Mas remova antes de testar, pois afetarão a comparação exata das saídas. Alternativamente, use stderr para debug (fprintf em C) pois os testes capturam apenas stdout.

## Testes

### Q: Meu simulador passa no teste 3 mas falha no teste 2. O que fazer?

**R:** Teste 3 é mais simples (memória suficiente). Teste 2 força substituição de páginas. Verifique:
- Algoritmo de substituição está correto?
- Está atualizando as tabelas de páginas corretamente?
- Está removendo a página antiga antes de adicionar a nova?

Execute manualmente no papel para entender o que deveria acontecer.

### Q: FIFO e Clock dão resultados diferentes. Isso é normal?

**R:** Nos testes fornecidos (1, 2, 3), FIFO e Clock dão resultados idênticos. Se estão diferentes, há um bug. Verifique:
- R-bit está sendo setado em todo acesso?
- Clock está zerando R-bit ao dar segunda chance?
- Ponteiro circular está avançando corretamente?

### Q: Posso criar meus próprios testes?

**R:** Sim! É altamente recomendado. Crie arquivos `config_4.txt`, `acessos_4.txt`, calcule manualmente a saída esperada, e teste. Isso ajuda a encontrar bugs e entender melhor os algoritmos.

## Relatório e Entrega

### Q: Quanto detalhe devo dar no relatório sobre estruturas de dados?

**R:** Seja específico mas conciso. Exemplo:
- "Usamos array de structs para tabela de páginas, indexado por número de página, armazenando frame, valid bit e R-bit"
- Explique **por que** escolheu (eficiência, simplicidade, etc.)

### Q: A análise comparativa deve ter quantos testes?

**R:** Mínimo 3 (os fornecidos), mas recomendamos criar pelo menos 1 teste próprio onde FIFO e Clock tenham resultados diferentes para ter uma análise mais rica.

### Q: O vídeo precisa mostrar o código?

**R:** Não necessariamente código linha por linha, mas mostre:
- Compilação funcionando
- Execução com ambos algoritmos
- Explicação da saída
- Breve explicação de uma decisão de design (ex: "escolhemos usar fila porque...")

Cerca de 5 minutos é suficiente.

### Q: Posso fazer o projeto sozinho?

**R:** Sim, mas lembre que o critério de avaliação não será flexibilizado para projetos individuais.

## Debugging

### Q: Meu programa compila mas dá erro ao executar. Como debugar?

**R:** Algumas técnicas:
1. **Prints de debug**: Mostre estado da memória após cada acesso
2. **Debugger**: Use gdb ou debugger da sua IDE
3. **Casos simples**: Teste com 1 processo, 2 frames, poucos acessos
4. **Validação manual**: Execute no papel (teste de mesa) e compare com seu programa

### Q: Como testar se o algoritmo Clock está correto?

**R:** Crie um teste onde você sabe exatamente o que deve acontecer:
- Encha a memória
- Acesse todas as páginas (todas R=1)
- Force um page fault
- Clock deve dar segunda chance a todas (zerar R) e voltar à primeira

Adicione prints mostrando R-bits para validar.

### Q: Estou recebendo "Segmentation Fault". O que fazer?

**R:** Comum em acesso a memória inválida:
- Acessando array fora dos limites
- Ponteiro não inicializado
- Acesso após free/delete

Use `valgrind` para identificar:
```bash
valgrind ./simulador fifo tests/config_1.txt tests/acessos_1.txt
```

## Conceitos

### Q: Não entendi a diferença entre página e frame. Pode explicar?

**R:**
- **Página**: Bloco de memória **virtual** (abstração). Cada processo tem suas páginas numeradas 0, 1, 2, ...
- **Frame**: Bloco de memória **física** (real, na RAM). Compartilhado entre todos processos.

Tabela de páginas mapeia: "Página X do processo Y está no frame Z".

### Q: Por que Clock é melhor que FIFO?

**R:** FIFO remove sempre a página mais antiga, mesmo se ela está sendo muito usada. Clock dá "segunda chance" a páginas acessadas recentemente (R=1), removendo primeiro páginas "velhas" que não foram recentemente acessadas. Isso geralmente resulta em menos page faults.

### Q: O que é o R-bit exatamente?

**R:** Referenced bit (bit de referência). Em hardware real, a MMU seta R=1 automaticamente quando a página é acessada. No nosso simulador, você deve fazer isso manualmente. Indica "esta página foi acessada recentemente". Usado pelo Clock para decidir a página vítima.

## GitHub Classroom

### Q: Como faço push do meu código?

**R:**
```bash
git add .
git commit -m "Implementação do simulador"
git push
```

### Q: Posso fazer múltiplos commits?

**R:** Sim! É até recomendado. Faça commits incrementais à medida que implementa funcionalidades:
- "Implementa tradução de endereços"
- "Implementa detecção de HIT"
- "Implementa FIFO"
- etc.

**Mais dúvidas?** Entre em contato com o professor: lucas.figueiredo@mackenzie.br