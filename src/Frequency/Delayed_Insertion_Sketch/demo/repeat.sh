#!/bin/bash

# 0.1から1.0まで0.1刻みでループ
for i in $(seq 0.1 0.1 1.0); do
  ./main ../../../../data/sx-stackoverflow_short.txt "$i" # 引数としてiを渡す
done
