#!/usr/bin/env bash
set -euo pipefail

# Caminho do binário (pode sobrescrever via: BIN=./build/os-deadlock-sim ./experiments.sh)
BIN="${BIN:-./os-deadlock-sim}"
OUT="${OUT:-out}"
mkdir -p "$OUT"

# ------------------------------------------------------------------
# Lista de combos: (modo, cenário, n, m)
# - Você pode ajustar/expandir essa grade à vontade.
# - n e m aqui sobrescrevem os defaults do main.c (--n/--m).
# ------------------------------------------------------------------
combos=(
  "ostrich tiny 2 2"
  "banker  tiny 2 2"

  "ostrich medium 6 3"
  "banker  medium 6 3"

  "ostrich deadlock 2 2"
  "banker  deadlock 2 2"

  "ostrich cycle-4 4 2"
  "banker  cycle-4 4 2"

  "ostrich hotspot 8 4"
  "banker  hotspot 8 4"

  "ostrich contention-90 10 3"
  "banker  contention-90 10 3"
)

# Cabeçalho do resumo (inclui n e m agora)
echo -e "mode\tscenario\tn\tm\ttotal\tgrants\tblocks\tsafety_calls\tns_total\tavg_ns\tdeadlocks\tt_first" > "$OUT/summary.tsv"

for entry in "${combos[@]}"; do
  # shellcheck disable=SC2086
  set -- $entry
  mode=$1; scen=$2; n=$3; m=$4

  log="$OUT/${scen}_${mode}.csv"
  json="$OUT/${scen}_${mode}.json"

  # Executa o simulador com sobrescrita de n e m
  line=$("$BIN" --mode "$mode" --scenario "$scen" --n "$n" --m "$m" \
               --log "$log" --metrics "$json" | tail -n1)

  # Extrai campos da linha "mode=... scenario=... | total=... grants=... blocks=... | ..."
  # Ex.: mode=BANKER scenario=medium | total=... grants=... blocks=... | safety_calls=... ns_total=... avg_ns=...
  modev=$(echo "$line" | awk -F'[ =|]+' '{print $2}')
  scenv=$(echo "$line" | awk -F'[ =|]+' '{print $4}')
  total=$(echo "$line" | awk -F'[ =|]+' '{print $6}')
  grants=$(echo "$line" | awk -F'[ =|]+' '{print $8}')
  blocks=$(echo "$line" | awk -F'[ =|]+' '{print $10}')

  safety_calls=0; ns_total=0; avg_ns=0; deadlocks=0; tfirst=0
  if [[ $modev == "BANKER" ]]; then
    safety_calls=$(echo "$line" | awk -F'[ =|]+' '{for(i=1;i<=NF;i++) if($i=="safety_calls") {print $(i+1); exit}}')
    ns_total=$(echo "$line" | awk -F'[ =|]+' '{for(i=1;i<=NF;i++) if($i=="ns_total") {print $(i+1); exit}}')
    # avg_ns pode não existir se safety_calls=0
    avg_ns=$(echo "$line" | awk -F'[ =|]+' '{for(i=1;i<=NF;i++) if($i=="avg_ns") {print $(i+1); found=1} } END{if(!found) print 0}')
  else
    deadlocks=$(echo "$line" | awk -F'[ =|]+' '{for(i=1;i<=NF;i++) if($i=="deadlocks") {print $(i+1); exit}}')
    tfirst=$(echo "$line" | awk -F'[ =|]+' '{for(i=1;i<=NF;i++) if($i=="t_first") {print $(i+1); exit}}')
  fi

  echo -e "$modev\t$scenv\t$n\t$m\t$total\t$grants\t$blocks\t$safety_calls\t$ns_total\t$avg_ns\t$deadlocks\t$tfirst" >> "$OUT/summary.tsv"
done

echo "OK! Resultados em: $OUT/summary.tsv  (e logs/json em $OUT/)"
