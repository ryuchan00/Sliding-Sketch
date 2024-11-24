#!/bin/bash

# 0.1から1.0まで0.1刻みでループ
for i in $(seq 10000 10000 50000); do
  echo "===================="
  ./main ../../../../data/sx-stackoverflow.txt 0.8 500000 "$i" > cycle_"$i".log # 引数としてiを渡す
  # ./main ../../../../data/artificial2.txt "$i" $1 # 引数としてiを渡す
  # ./main ../../../../data/artificial3.txt "$i" # 引数としてiを渡す
done
