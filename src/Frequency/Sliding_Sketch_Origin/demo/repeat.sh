#!/bin/bash

# 0.1から1.0まで0.1刻みでループ
for i in $(seq 1.1 0.1 2.0); do
  echo "===================="
  ./main ../../../../data/sx-stackoverflow.txt "$i" 500000 50000 # 引数としてiを渡す
  # ./main ../../../../data/artificial2.txt "$i" $1 # 引数としてiを渡す
  # ./main ../../../../data/artificial3.txt "$i" # 引数としてiを渡す
done
