#!/bin/bash

# This is to be run once the service starts
mkdir /app/Logs &> /dev/null
# start redis-server
nohup redis-server &> /app/Logs/redis.log &
# start web interface
nohup make start_web_service &>> /app/Logs/web.log &
# start decoder
nohup sh /app/Kaldi/run-service.sh &>> /app/Logs/kaldi.log &
echo "Service starting..."

wait
