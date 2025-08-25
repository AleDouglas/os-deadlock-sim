#!/usr/bin/env bash
set -euo pipefail

BIN=./os-deadlock-sim
OUT=out
mkdir -p "$OUT"

# combos: (modo, cenário)
combos=(
  "ostrich tiny"
  "banker  tiny"
  "ostrich medium"
  "banker  medium"
  "ostrich deadlock"
  "banker  deadlock"
)

# cabeçalho
echo -e "mode\tscenario\ttotal\tgrants\tblocks\tsafety_calls\tns_total\tavg_ns\tdeadlocks\tt_first" > "$OUT/summary.tsv"

for entry in "${combos[@]}"; do
  # shellcheck disable=SC2086
  set -- $entry
  mode=$1; scen=$2

  log="$OUT/${scen}_${mode}.csv"
  json="$OUT/${scen}_${mode}.json"

  line=$($BIN --mode "$mode" --scenario "$scen" --log "$log" --metrics "$json" | tail -n1)

  # Extrai campos da linha "mode=... scenario=... | total=... grants=... blocks=... | ..."
  modev=$(echo "$line" | awk -F'[ =|]+' '{print $2}')
  scenv=$(echo "$line" | awk -F'[ =|]+' '{print $4}')
  total=$(echo "$line" | awk -F'[ =|]+' '{print $6}')
  grants=$(echo "$line" | awk -F'[ =|]+' '{print $8}')
  blocks=$(echo "$line" | awk -F'[ =|]+' '{print $10}')

  safety_calls=0; ns_total=0; avg_ns=0; deadlocks=0; tfirst=0
  if [[ $modev == "BANKER" ]]; then
    safety_calls=$(echo "$line" | awk -F'[ =|]+' '{for(i=1;i<=NF;i++) if($i=="safety_calls") {print $(i+1); exit}}')
    ns_total=$(echo "$line" | awk -F'[ =|]+' '{for(i=1;i<=NF;i++) if($i=="ns_total") {print $(i+1); exit}}')
    avg_ns=$(echo "$line" | awk -F'[ =|]+' '{for(i=1;i<=NF;i++) if($i=="avg_ns") {print $(i+1); exit}}')
  else
    deadlocks=$(echo "$line" | awk -F'[ =|]+' '{for(i=1;i<=NF;i++) if($i=="deadlocks") {print $(i+1); exit}}')
    tfirst=$(echo "$line" | awk -F'[ =|]+' '{for(i=1;i<=NF;i++) if($i=="t_first") {print $(i+1); exit}}')
  fi

  echo -e "$modev\t$scenv\t$total\t$grants\t$blocks\t$safety_calls\t$ns_total\t$avg_ns\t$deadlocks\t$tfirst" >> "$OUT/summary.tsv"
done

echo "OK! Resultados em: $OUT/summary.tsv  (e logs/json em $OUT/)"
