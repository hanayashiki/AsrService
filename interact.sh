./path.sh
docker build -t aspnetapp . && docker run -v $ASR_SERVICE_PATH/Tools/cvte:/usr/local/kaldi/egs/cvte -t -i --entrypoint /bin/bash aspnetapp:latest 
