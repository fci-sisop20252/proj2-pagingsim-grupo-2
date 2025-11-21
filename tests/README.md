# Casos de Teste

Este diretório contém casos de teste para validar o funcionamento do seu simulador de memória virtual.

## Estrutura dos Testes

Cada teste consiste em 4 arquivos:

- **config_N.txt** - Arquivo de configuração (frames, tamanho de página, processos)
- **acessos_N.txt** - Sequência de acessos à memória
- **expected_fifo_N.txt** - Saída esperada quando executado com algoritmo FIFO
- **expected_clock_N.txt** - Saída esperada quando executado com algoritmo Clock

## Como Usar

### Executar o simulador

```bash
./simulador fifo tests/config_1.txt tests/acessos_1.txt > output.txt
```

### Comparar com saída esperada

```bash
diff output.txt tests/expected_fifo_1.txt
```

Se `diff` não mostrar nenhuma diferença, o teste passou!

### Testar todos os casos

```bash
# FIFO - Teste 1
./simulador fifo tests/config_1.txt tests/acessos_1.txt > output.txt
diff output.txt tests/expected_fifo_1.txt

# Clock - Teste 1
./simulador clock tests/config_1.txt tests/acessos_1.txt > output.txt
diff output.txt tests/expected_clock_1.txt

# Repita para testes 2 e 3
```

## Testes Disponíveis

### Teste 1: Exemplo Básico
- **Arquivo:** config_1.txt, acessos_1.txt
- **Acessos:** 8
- **Descrição:** Exemplo do enunciado com 8 frames, 2 processos
- **Objetivo:** Validar comportamento básico, HITs e PAGE FAULTs
- **Resultados:** 5 page faults (FIFO = Clock)

### Teste 2: Memória Pequena
- **Arquivo:** config_2.txt, acessos_2.txt
- **Acessos:** 10
- **Descrição:** Apenas 4 frames, 3 processos, força substituição
- **Objetivo:** Testar algoritmos de substituição com memória limitada
- **Resultados:** 10 page faults (FIFO = Clock)

### Teste 3: Caso Simples
- **Arquivo:** config_3.txt, acessos_3.txt
- **Acessos:** 7
- **Descrição:** 1 processo, memória suficiente
- **Objetivo:** Validar lógica básica de tradução e detecção de HITs
- **Resultados:** 4 page faults (FIFO = Clock)

### Teste 4: Grande - Múltiplos Padrões ⭐
- **Arquivo:** config_4.txt, acessos_4.txt
- **Acessos:** 100
- **Descrição:** 10 frames, 4 processos, padrões variados de acesso
- **Objetivo:** Teste abrangente com working sets, thrashing e acessos aleatórios
- **Resultados:** FIFO = 39 page faults, Clock = 38 page faults
- **Observação:** Clock tem 1 fault a menos devido ao R-bit

### Teste 5: Stress - Memória Sob Pressão ⭐⭐
- **Arquivo:** config_5.txt, acessos_5.txt
- **Acessos:** 150
- **Descrição:** Apenas 6 frames, 4 processos (18 páginas), muita competição
- **Objetivo:** Teste de stress com substituição constante
- **Resultados:** FIFO = 78 page faults, Clock = 77 page faults
- **Observação:** Alta taxa de page faults devido à memória limitada

### Teste 6: Demonstração FIFO vs Clock ⭐
- **Arquivo:** config_6.txt, acessos_6.txt
- **Acessos:** 41
- **Descrição:** 3 frames, padrão específico para mostrar vantagem do Clock
- **Objetivo:** Demonstrar conceito de "páginas quentes" e segunda chance
- **Resultados:** Verificar se Clock tem menos faults que FIFO

## Importante

### Formato de Saída

O formato de saída deve ser **EXATAMENTE** como especificado no ENUNCIADO.md:

- Espaços, pontuação e quebras de linha devem ser idênticos
- Maiúsculas e minúsculas importam
- Use `diff` para verificar que não há nenhuma diferença

### Testes Adicionais

Na avaliação, seu simulador será testado com casos **adicionais** não fornecidos aqui. Certifique-se de que seu código:

- Funciona para qualquer configuração válida
- Não tem valores hardcoded específicos para estes testes
- Implementa corretamente a lógica de tradução, page faults e algoritmos

### Criar seus Próprios Testes

Recomendo criar casos de teste adicionais para validar seu simulador:

1. Crie novos arquivos config_X.txt e acessos_X.txt
2. Execute manualmente (no papel) para calcular a saída esperada
3. Compare com a saída do seu simulador
4. Isso ajuda a encontrar bugs e entender melhor os algoritmos

## Debugging

Se os testes estão falhando:

1. **Verifique o formato de saída** - use `diff` para ver exatamente onde está a diferença
2. **Execute manualmente** - simule os acessos no papel para entender o que deveria acontecer
3. **Use prints de debug** - adicione prints para ver o estado da memória e tabelas de páginas
4. **Teste casos simples primeiro** - comece com teste 3 (mais simples) antes de tentar teste 2 (mais complexo)

Boa sorte!
