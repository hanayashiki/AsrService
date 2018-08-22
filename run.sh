#!/bin/bash
source path.sh
mkdir Logs &> /dev/null

FLAGS="-p 80:80"
TEST=false

if [ "$1" = "interact" ] 
then
	FLAGS="-t -i $FLAGS"
	ENTRYPOINT="--entrypoint /bin/bash"
elif [ "$1" = "deploy" ]
then
	FLAGS="--detach $FLAGS"
	TEST=true
	ENTRYPOINT="--entrypoint ./init.sh"
else
	echo "Usage: $0 (interact|deploy)"
	exit 1
fi

echo "Using docker flags: $FLAGS $ENTRYPOINT"

docker build -t wangchenyu/aspnetapp . && \
	docker run -v $ASR_SERVICE_PATH/Tools/cvte:/usr/local/kaldi/egs/cvte \
	-v $ASR_SERVICE_PATH/Logs:/app/Logs $FLAGS $ENTRYPOINT \
	wangchenyu/aspnetapp:latest 

if [ "$TEST" = "true" ]
then
	python3 APITest/autoapi.py waitandpoll
fi
