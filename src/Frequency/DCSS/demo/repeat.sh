#!/bin/bash

# 0.1から1.0まで0.1刻みでループ
for i in $(seq 0.1 0.1 2.0); do
  echo "===================="
  # output=$(./main ../../../../data/sx-stackoverflow_500_000.txt "$i" 500000 50000) # ./mainの出力を変数に保存
  output=$(./main ../../../../data/sx-stackoverflow.txt "$i" 5000000 50000) # ./mainの出力を変数に保存
  # output=$(./main ../../../../data/webdocs.dat "$i" 5000000 50000) # ./mainの出力を変数に保存
  #./main ../../../../data/sx-stackoverflow_500_000.txt "$i" 500000 50000 # 引数としてiを渡す
  # ./main ../../../../data/artificial2.txt "$i" $1 # 引数としてiを渡す
  # ./main ../../../../data/artificial3.txt "$i" # 引数としてiを渡す

  printf "%s\n" "$output"

  # 抽出と記録
  # value=$(echo "$output" | awk -F',' '/Sl-CM,5000000/ {print $3}')
  value=$(echo "$output" | awk -F',' '/ARE,/ {print $3}')
  if [[ ! -z "$value" ]]; then
    results="${results}${value},"
  fi
done

# 最後のカンマを削除してダンプ
results=${results%,} # 最後のカンマを削除
echo "DCSS Collected results: $results"