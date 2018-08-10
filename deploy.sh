source path.sh
chmod +x $ASR_SERVICE_PATH/Tools/cvte/predict.sh
docker build -t wangchenyu/aspnetapp . && docker run -v $ASR_SERVICE_PATH/Tools/cvte:/usr/local/kaldi/egs/cvte aspnetapp -d -p 8080:80 aspnetapp:latest

