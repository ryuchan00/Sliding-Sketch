#!/bin/bash

input_num=5000000
# element_count=1000
element_counts=(500 1000 1500 2000 2500 3000 3500 4000 4500 5000)
mymemory=0.4

# 0.1から1.0まで0.1刻みでループ
# for i in $(seq 0.4 0.1 2.0); do
for element_count in "${element_counts[@]}"; do
  echo "Element Count: $element_count"
  echo "===================="
  # output=$(./main ../../../../data/sx-stackoverflow_500_000.txt "$i" $input_num 5000) # ./mainの出力を変数に保存
  # output=$(./main ../../../../data/sx-stackoverflow.txt "$i" $input_num $element_count) # ./mainの出力を変数に保存
  output=$(./main ../../../../data/sx-stackoverflow.txt $mymemory $input_num $element_count) # ./mainの出力を変数に保存
  # ./main ../../../../data/artificial2.txt "$i" $input_num # 引数としてiを渡す
  # ./main ../../../../data/artificial3.txt "$i" # 引数としてiを渡す
  printf "%s\n" "$output"

  # 抽出と記録
  value=$(echo "$output" | awk -F',' '/Sl-CM,5000000/ {print $3}')
  if [[ ! -z "$value" ]]; then
    results="${results}${value},"
  fi
done

# 最後のカンマを削除してダンプ
results=${results%,} # 最後のカンマを削除
echo "DISS: $results"