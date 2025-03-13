#!/bin/bash

# 定义要运行的客户端数量
NUM_CLIENTS=36

# 循环启动客户端
for ((i=1; i<=NUM_CLIENTS; i++))
do
    echo "Starting syncClient $i"
    ./build/syncClient &  # 将 syncClient 放到后台运行
done

# 等待所有客户端完成
wait

echo "All syncClient processes have finished."