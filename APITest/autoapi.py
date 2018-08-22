#!python3

import json
import requests
import gevent
import sys
import time
from gevent import monkey
monkey.patch_socket()

url = 'http://localhost:80/Decoding/Decode'

time_list = []

def post(wave_path: str):
    start = time.time()
    r = requests.post(url, files={'wave': open(wave_path, 'rb')}, timeout=(0.01, 20))
    time_list.append(time.time() - start)
    return r.text

def checkOk(text, expect='Ok'):
    j = json.loads(text)
    if j['Message'].lower().find(expect.lower()) == -1:
        print('Message: ' + j['Message'] + ', ' + 'expect: ' + expect)
        return False
    return True

def test1(wav_path, expect='Ok'):
    text = post(wav_path)
    print(text);
    return checkOk(text, expect)

if len(sys.argv) < 2:
    print("not enough args")
    sys.exit(-1)


if sys.argv[1] == "single_normal":
    ok = test1('Tools/cvte/s5/data/wav/00030/2017_03_07_16.59.42_5013.wav')
    if not ok:
        sys.exit(-1)
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
    ok = test1('APITest/empty.wav', 'bad wave')
    if not ok:
        exit(-1)
elif sys.argv[1] == "bad_url":
    r = requests.post(url, files={'file': open('Tools/cvte/s5/data/wav/00030/2017_03_07_16.59.42_5013.wav', 'rb')})
    print(r.text)
elif sys.argv[1] == "big":
    ok = test1("APITest/big.wav", "smaller")
    if not ok:
        sys.exit(-1)
elif sys.argv[1] == "waitandpoll":
    print("Waiting for Kaldi decoder to init...")
    print("If taken too long, see Logs. There might be failures. ")
    while True:
        try:
            ok = checkOk(post('Tools/cvte/s5/data/wav/00030/2017_03_07_16.59.42_5013.wav'))
            if ok:
                print("Service is ready")
                break
            else:
                print(".", end='')
            time.sleep(1)
        except requests.exceptions.ConnectionError:
            print("Connectiong failed, retrying...")
            time.sleep(3)
            continue
        except requests.exceptions.ReadTimeout:
            print("Waiting for Kaldi to start...") 
            time.sleep(3)
            continue
        except Exception as e:
            print(e)
            time.sleep(3)
            continue
else:
    print("args not supported")
