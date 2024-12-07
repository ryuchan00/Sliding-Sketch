#!/bin/bash

input_num=500000

# 0.1から1.0まで0.1刻みでループ
for i in $(seq 0.4 0.1 2.0); do
  echo "===================="
  ./main ../../../../data/sx-stackoverflow.txt "$i" $input_num 5000 # 引数としてiを渡す
  # ./main ../../../../data/artificial2.txt "$i" $input_num # 引数としてiを渡す
  # ./main ../../../../data/artificial3.txt "$i" # 引数としてiを渡す
done
