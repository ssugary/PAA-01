# A Comparative Study of $k$-opt Local Search Combined with Ruin-and-Recreate For the Orienteering Problem

(depois irei colocar em inglês...)

## Introduction

O Problema de Orientação (Orienteering Problem) é um problema NP-Hard que consiste na busca por um caminho entre dois vértices, de maneira que maximiza uma pontuação 
dado um limite de tempo (nesse caso, denotado por TMax). Dado o tempo limitado, os caminhos não precisam percorrer todos os vértices.

Para modelar este problema, definimos um grafo G = {V, E}. onde denotamos V como o conjunto de todos os vértices e E o conjunto de todas as ligações entre esses vértices.

Cada vértice $i$ deverá ter uma pontuação $S_i \ge 0$ associada, enquanto o vértice inicial $1$ e o vértice final $n$ terão pontuações iguais a $0$.
Além disso, o custo entre cada aresta (i, j) deverá ser dado pela distância euclidiana $d_{ij}$.

Logo, o objetivo se torna encontrar um caminho entre os vértices $1$ e $n$, por meio de um subconjunto de vértices que irá maximizar a pontuação total sem violar a restrição de tempo. 

## Proposed Methodology

Nesse estudo iremos analisar como algumas heurísticas construtivas se comportam em junção da extratégia *Ruin-and-Recreate* e a metaheurística *GRASP* para a geração de rotas iniciais otimizadas.

Inicialmente, as heurísticas escolhidas para esse estudo são:

- 2-opt
- 3-opt
- 4-opt

Por conta da complexidade alta do 4-opt, iremos analisar as implementações de Best e First Improvement dela, enquanto as 2-opt e 3-opt utilização apenas a extratégia Best Improvement.

### $k$-opts Heuristics

Essas heurísticas consistem em realizar pertubações em uma rota, removendo $k$ arestas dela e reconectando as arestas invertendo a ordem dos vértices. 

Por exemplo, caso eu tenha uma rota A - B - C - D - E, o algoritmo iria pegar 2 arestas, digamos AB e DE, e iria ver se a distância seria melhor caso seguissemos a rota por AD e BC, comparanto a rota inicial com A - D - C - B - E 

O mesmo aconteceria para o 3-opt e 4-opt, mas, como a complexidade aumentaria muito (teríamos que calcular cerca de 8 e 25 rotas respectivamente a cada iteração), iremos considerar apenas as seguintes rotas:

- Para o 3-opt, calculamos apenas as rotas que não são calculadas por 2-opt e que não fazem inversões simples (pega uma aresta AB e transforma em BA).

- Para o 4-opt, calculamos apenas o menor movimento que não pode ser alcançado pelos 2-opt e 3-opt (chamados de Double Bridge)

Essas escolhas deixarão as comparações mais "justas", por diminuir a complexidade desses algoritmos, também permitindo a combinação dessas heurísticas sem desperdiçar cálculos.

Note que nenhuma dessas heurísticas irá afetar na pontuação das rotas, apenas diminuirá o tempo gasto, permitindo a exploração de novos vértices.

### Ruin-and-Recreate Heuristic

Essas Heurísticas consiste em duas partes principais:

- Ruin: Onde deletamos uma parcela de vértices
- Recreate : Onde reconstruímos a rota destruída

Com esse objetivo, o Ruin foi implementado de forma que ele escolha $30\%$ dos vértices de forma aleatória e remova eles, assim podemos fugir de ótimos locais testando novos vértices com a parte de *Recreate*.

Para os vértices sejam escolhidos de forma consistente, implementamos o Recreate utilizando uma Heurística de Inserção Gulosa por Custo-Benefício, assim consideramos apenas os vértices que possuam uma relação $\dfrac{score}{distance}$ acima das demais e que não ultrapassem o TMax.


Logo, essa heurística consegue pertubar rotas e fugir de ótimos locais, permitindo exploremos áreas que alterem tanto a pontuação quanto a distância total da rota. Pela natureza da heurística, ela acaba combinando perfeitamente com heurísticas que alteram apenas as distâncias, já que elas conseguem aumentar a quantidade de vértices candidatos para a parte de Recreate.

### Combining the Heuristics

Com isso, os testes serão realizados seguindo esse padrão:

```

Matriz_de_resultados resultados[qnt_de_heuristicas][qnt_de_testes].
Vetor_de_heuristicas opt[qnt_de_heuristicas].

Gera rota r.
Para cada i de 1 até qnt_de_heuristicas faça:
    Rota r' = opt[i](recreate(ruin(opt[i](r)))).
    Se r.fitness < r'.fitness faça:
        inserir r' em resultados[i].
    Senão:
        inserir r  em resultados[i].
    
```

Com isso, aplicamos as $k$-opt antes e depois do RR, fazendo com que o RR tenha mais opções de vértices e a rota esteja no seu ótimo local.

No fim, eu escolhi o algoritmo $GRASP$ para a geração inicial das rotas e combinei as heurísticas da seguinte forma:

- GRASP Padrão
- GRASP + Ruin + Recreate
- GRASP + 2-opt + Ruin + Recreate
- GRASP + 3-opt + Ruin + Recreate

Também separei o 4-opt em outros 4 casos, onde em casa um eu aplicava as versões Best e First Improvement em ordens diferentes, ficando:

- GRASP + 4-opt-BIBI + Ruin + Recreate
- GRASP + 4-opt-FIFI + Ruin + Recreate
- GRASP + 4-opt-FIBI + Ruin + Recreate
- GRASP + 4-opt-BIFI + Ruin + Recreate


## Results

Abaixo eu deixo alguns resultados obtidos utilizando uma população de 100 indivíduos no Data Set Tsiligirides1_85:

| Algorithm | Median | Max | Delta Fit | Average Time | Overhead |
| :--- | :---: | :---: | :---: | :---: | :---: |
| Grasp | 205.0 | 230.0 | +0.00% | 0.000 ms | 0.00x |
| Grasp + Ruin + Recreate | 255.0 | 285.0 | +24.39% | 0.011 ms | 0.69x |
| Grasp + 2-OPT-BIBI + Ruin + Recreate | 280.0 | 285.0 | +36.59% | 0.016 ms | 1.00x (target)|
| Grasp + 3-OPT-BIBI + Ruin + Recreate | 280.0 | 285.0 | +36.59% | 0.153 ms | 9.60x |
| Grasp + 4-OPT-BIBI + Ruin + Recreate | 280.0 | 285.0 | +36.59% | 0.456 ms | 28.69x |
| Grasp + 4-OPT-FIFI + Ruin + Recreate | 285.0 | 285.0 | +39.02% | 0.123 ms | 7.74x |
| Grasp + 4-OPT-FIBI + Ruin + Recreate | 285.0 | 285.0 | +39.02% | 0.294 ms | 18.48x |
| Grasp + 4-OPT-BIFI + Ruin + Recreate | 285.0 | 285.0 | +39.02% | 0.264 ms | 16.61x |

Esse experimento foi realizado em um notebook Acer Nitro ANV15-51 com um processador 13th Gen Intel® Core™ i5-13420H × 12 e 16.0 GB de RAM, rodando no Fedora Linux 42 (Workstation Edition). Além disso, todos os testes foram executados em regime single-thread, sem o uso de aceleração por GPU. O código em C++20 foi compilado via GCC utilizando as otimizações das flags `-O3 -march=native -flto -ffast-math`.


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

* **Linha 1:** `<TMax>` `<Parametro_Lixo>`
* **Linha 2:** `<x_inicio>` `<y_inicio>` `0` (Vértice inicial, lucro = 0)
* **Linha 3:** `<x_fim>` `<y_fim>` `0` (Vértice final, lucro = 0)
* **Linhas 4+:** `<x_i>` `<y_i>` `<lucro_i>` (Coordenadas e lucro de cada cliente)

ex:

```text
85.0    1
10.5    14.4    0
11.2    14.1    0
18.0    15.9    10
```

As instâncias utilizadas foram retiradas das instâncias de **Tsiligirides** (1984) e **Chao et al** (1996).

Após isso, rode via linha de comando da seguinte forma:

```bash
./build/OPTests <caminho_da_instância> <arquivo_de_saída>
```

Caso o parâmetro <arquivo_de_saída> esteja vazio, o programa criará um arquivo `teste.txt` com as informações obtidas além dos resultados no terminal. 

## Licence

Copyright (c) 2026 ssugary

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
