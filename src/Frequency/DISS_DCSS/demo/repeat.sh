#!/bin/bash

input_num=500000

# 0.1から1.0まで0.1刻みでループ
for i in $(seq 0.4 0.1 2.0); do
  echo "===================="
  output=$(./main ../../../../data/sx-stackoverflow_500_000.txt "$i" $input_num 5000) # ./mainの出力を変数に保存
  # ./main ../../../../data/artificial2.txt "$i" $input_num # 引数としてiを渡す
  # ./main ../../../../data/artificial3.txt "$i" # 引数としてiを渡す
  printf "%s\n" "$output"

  # 抽出と記録
  value=$(echo "$output" | awk -F',' '/Sl-CM,500000/ {print $3}')
  if [[ ! -z "$value" ]]; then
    results="${results}${value},"
  fi
done

# 最後のカンマを削除してダンプ
results=${results%,} # 最後のカンマを削除
echo "DCSS AND DISS: $results"