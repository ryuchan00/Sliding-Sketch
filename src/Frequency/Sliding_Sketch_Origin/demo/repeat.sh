#!/bin/bash

# 0.1から1.0まで0.1刻みでループ
for i in $(seq 0.1 0.05 0.4); do
  ./main ../../../../data/sx-stackoverflow.txt "$i" # 引数としてiを渡す
done
