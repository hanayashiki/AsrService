#!python3

import requests
import gevent
import sys
from gevent import monkey
monkey.patch_socket()

url = 'http://localhost:5000/Decoding/Decode'

def post(wave_path: str):
    r = requests.post(url, files={'wave': open(wave_path, 'rb')})
    return r.text

def test1(wav_path):
    print(post(wav_path));

if len(sys.argv) < 2:
    print("not enough args")
    sys.exit(-1)


if sys.argv[1] == "single_normal":
    test1('Tools/cvte/s5/data/wav/00030/2017_03_07_16.59.42_5013.wav')

elif sys.argv[1] == "parallel":
    wavs = ['Tools/cvte/s5/data/wav/00030/2017_03_07_16.59.42_5013.wav'] * 10

    jobs = [ gevent.spawn(test1, wav) for wav in wavs ]
    gevent.joinall(jobs)
else:
    print("args not supported")
