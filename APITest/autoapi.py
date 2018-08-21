#!python3

import requests
import gevent
import sys
import time
from gevent import monkey
monkey.patch_socket()

url = 'http://localhost:5000/Decoding/Decode'

time_list = []

def post(wave_path: str):
    start = time.time()
    r = requests.post(url, files={'wave': open(wave_path, 'rb')})
    time_list.append(time.time() - start)
    return r.text

def test1(wav_path):
    print(post(wav_path));

if len(sys.argv) < 2:
    print("not enough args")
    sys.exit(-1)


if sys.argv[1] == "single_normal":
    test1('Tools/cvte/s5/data/wav/00030/2017_03_07_16.59.42_5013.wav')

elif sys.argv[1] == "parallel":
    if len(sys.argv) < 3:
        count = 10
    else:
        count = int(sys.argv[2])
    wavs = ['Tools/cvte/s5/data/wav/00030/2017_03_07_16.59.42_5013.wav'] * int(count)

    jobs = [ gevent.spawn(test1, wav) for wav in wavs ]
    gevent.joinall(jobs)
    print("avg rtt: %fs" % (sum(time_list) / len(time_list)))
elif sys.argv[1] == "empty":
    test1('APITest/empty.wav')
elif sys.argv[1] == "bad_url":
    r = requests.post(url, files={'file': open('Tools/cvte/s5/data/wav/00030/2017_03_07_16.59.42_5013.wav', 'rb')})
    print(r.text)
elif sys.argv[1] == "big":
    print(post("APITest/big.wav"))
else:
    print("args not supported")
