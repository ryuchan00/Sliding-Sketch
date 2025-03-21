#!/bin/bash

input_num=5000000
cycle=50000
element_count=1000
mymemory=1

# 0.1から1.0まで0.1刻みでループ
for i in $(seq 1.0 1.0 5.0); do
  echo "===================="
  # output=$(./main ../../../../data/sx-stackoverflow_500_000.txt "$i" $input_num 5000) # ./mainの出力を変数に保存
  # output=$(./main ../../../../data/sx-stackoverflow.txt $mymemory $input_num $cycle) # ./mainの出力を変数に保存
  output=$(./main ../../../../data/webdocs.dat $mymemory $input_num $cycle) # ./mainの出力を変数に保存
  # ./main ../../../../data/artificial2.txt "$i" $input_num # 引数としてiを渡す
  # ./main ../../../../data/artificial3.txt "$i" # 引数としてiを渡す
  printf "%s\n" "$output"

  # 抽出と記録
  value=$(echo "$output" | awk -F',' '/Insertion Time/ {print $2}')
  # value=$(echo "$output" | awk -F',' '/ARE,/ {print $3}')
  if [[ ! -z "$value" ]]; then
    results="${results}${value},"
  fi

  value2=$(echo "$output" | awk -F',' '/Query Time/ {print $2}')
  # value=$(echo "$output" | awk -F',' '/ARE,/ {print $3}')
  if [[ ! -z "$value" ]]; then
    results2="${results2}${value2},"
  fi
done

# 最後のカンマを削除してダンプ
results=${results%,} # 最後のカンマを削除
echo "Insertion Time,$results"

results2=${results2%,} # 最後のカンマを削除
echo "Query Time,$results2"