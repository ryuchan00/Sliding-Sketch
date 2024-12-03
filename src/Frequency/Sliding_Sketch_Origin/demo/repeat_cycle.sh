#!/bin/bash

# 0.1から1.0まで0.1刻みでループ
for j in $(seq 0.4 0.1 2.0); do
  for i in $(seq 10000 10000 80000); do
    echo "===================="
    ./main ../../../../data/sx-stackoverflow.txt "$j" 500000 "$i" >> memory_"$j"_change_cycle.log # 引数としてiを渡す
    # ./main ../../../../data/artificial2.txt "$i" $1 # 引数としてiを渡す
    # ./main ../../../../data/artificial3.txt "$i" # 引数としてiを渡す
  done
done
