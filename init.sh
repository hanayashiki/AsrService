#!/bin/bash

# This is to be run once the service starts
# start redis-server
mkdir /app/Logs &> /dev/null
nohup redis-server &> /app/Logs/redis.log &
nohup make start_web_service &>> /app/Logs/web.log &
nohup sh /app/Kaldi/run-service.sh &>> /app/Logs/kaldi.log &
echo "Service starting..."

wait
