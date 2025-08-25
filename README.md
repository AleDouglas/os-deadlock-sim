### Tiny no BANQUEIRO (mede overhead)
```
./os-deadlock-sim --mode banker --scenario tiny --log tiny_banker.csv --metrics tiny_banker.json
```

```
./os-deadlock-sim --mode banker --scenario medium --log tiny_banker.csv --metrics tiny_banker.json
```


### Deadlock clássico no OSTRICH (detector liga)
```
./os-deadlock-sim --mode ostrich --scenario deadlock --log deadlock_ostrich.csv --metrics deadlock_ostrich.json
```


### Deadlock clássico no BANQUEIRO (evita deadlock)
```
./os-deadlock-sim --mode banker --scenario deadlock --log deadlock_banker.csv --metrics deadlock_banker.json
```


Se algo fugir muito do esperado, cheque:
* Max = Allocation_inicial + soma(das requisições) (para o Banker não bloquear à toa).
* detect_deadlock sendo chamado no sim_run (condição de “sem progresso” + algum BLOCKED).



### Usando experiments.sh para gerar tabela

```
make
chmod +x experiments.sh
./experiments.sh
column -t -s$'\t' out/summary.tsv
```



## Alterando Instruções:
### Quantidade de Processos / Quantidade de Recursos



Variáveis/arquivos:

* resources.h: aumente #define MAX_P (processos) e/ou #define MAX_R (tipos de recurso).

Onde usar em runtime: passe novos n e m em sim_init(&S, n, m, ...).
* n = Quantidade de Processos Ativos
* m = Tipos de Recursos

Atenção: safety_check/detector usam arrays locais Work[MAX_R] e Finish[MAX_P]. Se MAX_* crescer muito, considere trocar por alocação dinâmica (heap) para não estourar stack.

experiments.sh/CLI: adicione flags --n e --m (se quiser variar sem recompilar).


### Scripts mais longos

Variáveis/arquivos:

* process.h (ReqList): aumente #define MAX_REQS.
* Loaders: empurre mais reqlist_push(...).
* Consistência (Banker): mantenha Max = Allocation_inicial + soma(script) para cada processo.


### Modo de Simulação

O que muda: política (Ostrich vs Banker, e até detecção).

Variáveis/arquivos:

* Mode (enum) em resources.h/simulator.h.
* dispatcher.c: já decide por modo; para adicionar um modo "DETECTION" (resolver só detectando), crie um branch que nunca concede requisições inseguras, só roda detector para métrica.
* experiments.sh: acrescente os novos modos na matriz.
