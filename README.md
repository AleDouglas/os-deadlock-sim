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
chmod +x experiments.sh
./experiments.sh
column -t -s$'\t' out/summary.tsv
```
