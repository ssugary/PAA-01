# Orienteering Problem Applied on the Procedural Dungeon Generation Problem

## Requirements

- **CMake** (min version 3.16)
- **C++20 Compiler**

## Build

Para preparar o CMake use:

```
cmake -S . -B build
cmake --build build
```

## Run

Para executar o programa, você deverá ter uma testbench pronta para esse problema seguindo o modelo:

* **Linha 1:** `<TMax>`
* **Linha 2:** `<Dificuldade_inicio>` `0.0` `0` (Vértice inicial, lucro = 0, tipo = 0)
* **Linha 3:** `<Dificuldade_final>` `10.0` `1`(Vértice final, lucro = 0)
* **Linhas 4+:** `<Dificuldade_i>` `<lucro_i>` `<tipo_i>`(Dificuldade, lucro de cada e tipo da Sala)

ex:

```text
48.56
0.00 0.00 0
10.00 0.00 1
6.80 65.12 7
4.71 75.86 7
```

As instâncias utilizadas foram retiradas do gerador de instâncias `InstandeGenerator.cpp` que poderá ser executado da seguinte forma:

```bash
./build/InstanceGen <n_salas> <seed> <fator_orcamento> <saida.txt>  <TMax_fixo>
```

Onde `seed` é o parâmetro de semente aleatória na criação da masmorra, `fator_orcamento` é um o parâmetro `f` onde, para a instância, definirá o Tmax como `Tmax = f * minValidPathCost`, onde minValidPathCost é o caminho de menor dificuldade entre o começo e o fim da dungeon. A variavel `fator_orcamento` é ignorada quando `TMax fixo` for escolhido.


Após isso, com uma instância pronta, rode via linha de comando da seguinte forma:

```bash
./build/OPTests <caminho_da_instância> <arquivo_de_saída>
```

A saída do programa resultará na tabela:

```
N (Salas)  TMax Médio	GRASP Médio	Final Médio	Ganho Médio (%) Tempo Médio (ms)
 --             --           --          --         --              --
```

Caso o parâmetro <arquivo_de_saída> esteja vazio, o programa criará um arquivo `teste.txt` com as informações obtidas além dos resultados no terminal. 
