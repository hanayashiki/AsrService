An ASR (Automatic Speech Recognition) Service based on [kaldi](https://github.com/kaldi-asr/kaldi)   
The [model](http://kaldi-asr.org/models/m2) for decoding is trained by CVTE.

## Purpose

This service is intended to decode **Mandarin Chinese** speech into Chinese text.   

## How to deploy

### Build docker image

This service is aimed to help you to build a **dockerized** ASR app free of any trouble, just follow the steps and wait for the downloading and compiling.

This server runs on Ubuntu 16.04 and similar Linux systems, with large memory capacity >= 64G.

Install [docker-ce](https://docs.docker.com/install/linux/docker-ce/ubuntu/) >= 18.06.0

If you have installed docker, just 

```
cd AsrService
chmod +x interact.sh
./interact.sh
```

And the build will start. It will pull some images like `microsoft/dotnet:aspnetcore-runtime` and `microsoft/dotnet:sdk`.

It will build **kaldi** first, which may cost a lost of time and fail due to not enough memory. After **kaldi** echos `done`, your terminal may lag for a few minutes but it does not matter, just sit.

After kaldi is built successfully, it will download some other easier dependencies.

And finally, it will compile the ASP.NET app as the **web interface**, as described below. Finally it will build **decoder**. 

### Download CVTE model

http://kaldi-asr.org/models/m2

*TODO*

## Rest API

This service takes in speech in \*.wav format, which should be exactly **16 KHz** and **16-bit**.

The server accepts POST request, with field of "wave" in the body, carrying the wave file with standards above.

The server sends response in following format:

```
{
  "Id": "6b239c12-a5dc-47f8-a17a-c31a6e34bb7e",
  "Text": "关于黄款汇率希望大家不要被误导当然这个火鸡的答案并不对",
  "Message": "Ok"
}
```

The first field `Id` is the id generated for the speech (GUID), with which we can do some tracing in the log;

The second field `Text` is the Chinese text of the given speech.

The third field carries some messages if the decoding failed.

for example:

*TODO*

## Structure

This server comprises mainly three parts: (1) ASP.NET **web interface**, (2) a **message queue** implemented with redis, (3) **decoder** implemented with [kaldi](https://github.com/kaldi-asr/kaldi) .

(1) is implemented in `AsrService/Service` and `AsrService/Core` with .NET Core 2.0 in C# language.
(3) is implemented in `AsrService/Kaldi` with C++ calling an Kaldi API packed by myself in `https://github.com/hanayashiki/kaldi`.

+ the web interface accepts post requests as decribed above and sends message `Speech` to the message queue. In fact the message queue consists of two redis lists. List `SpeechQueue` is for web interface to place `Speech` and the decoder will fetch `Speech` from `SpeechQueue`. Every time the web interface places a `Speech`, it will signify the decoder to work by publishing a blank message on redis channel `SpeechSignal` and the decoder will know to get to work.

+ the message queue consists of two [queues](https://redis.io/topics/data-types) and two [channels](https://redis.io/topics/pubsub). The aforementioned queue `SpeechQueue`, is for the web interface to place speech works and for decoder to fetch them. And another queue `TextQueue`, is for the decoder to provide decoding result and for the web interface to fetch them. Each queue has a corresponding channel, namely `SpeechSignal` and `TextSignal`, which is used to wake up threads of web interface waiting for decoding result and threads of decoder for decoding works.

+ the decoder consists of a decoding model and a few threads. The threads wait for signal of `SpeechSignal` and will use the shared decoding model to decode the wave. This will use an [API](https://github.com/hanayashiki/kaldi/blob/9f7329f53174ba874156cd56ef4764234985c4f5/src/online2/online2-wav-nnet3-latgen-faster-api.h) implemented in another repository, which is a little encapsulation for kaldi's online decoding. For more information about decoding, see [kaldi's documentation](http://kaldi-asr.org/doc/online_decoding.html). 

## Configuration

For the minimalistic control, the service uses three configuration files.

`async_decoder_config.json` configures "(1) web interface"

+ async_decoder_config.json
  ```javascript 
  {
    "ClearRedis": true,                         // Where we execute `FLUSHALL` to redis
    "MaxRetrials": 3,                           // The maximum time of retrials of getting message from decoder 
    "DecodingTimeoutMs": 300000,                // Longest waiting time after a single `Speech` is put into the queue 
    "Redis": {                                  // redis connection
      "IP": "127.0.0.1",
      "Port": 6379
    },
    "EnableFakeDecoder": false                  // `enable fake decoder` means another decoder, will listen on `SpeechSignal` for work and provides fast but fake results. This is useful to test only "(1) web service" and "(2) message queue"`.
  }

  ```
   This configuration is for "(1) web interface". Run web interface with
   ```
   cd AsrService
   dotnet Service/bin/Debug/netcoreapp2.0/Service.dll --decoder-config=path/to/async_decoder_config.json
   ```

The following files configures "(3) decoder"

+ kaldi_config.json
  ```javascript
  {
      "max_wave_size_byte": 1000000,            // The maximum size of a wave file
      "redis_key": "",                          // Path to the key
      "redis_port": 6379,                       
      "redis_server": "127.0.0.1",
      "thread_expires_after_decoding": true,    // Whether the thread quits looping for works. This is useful if you want to detect memory leaks with valgrind or something.
      "use_kaldi": true,                        // Whether we use kaldi. If `false`, we will save a lot of time from loading the model.
      "workers": 100                            // How many threads are doing works. Parallelism can accommodate more requests. 
  }
  ```

+ decoder_config.json

This configuration file is very tricky and most of the explanation must be found in http://kaldi-asr.org/doc/online_decoding.html,
because this is generally transcripting shell arguments to json. But you can follow the steps above and just modify some parameter for adjustment of performance.

```javascript
  {
      "acoustic_scale": 1.0,
      "add_pitch": false,
      "beam": 3.0,    // the bigger, the better result, the slower
      "chunk_length_secs": -1.0,
      "config": "/usr/local/kaldi/egs/cvte/s5/conf/online.conf",
      "do_endpointing": false,
      "feature_type": "fbank",
      "frames_per_chunk": 50,
      "fst_in": "/usr/local/kaldi/egs/cvte/s5/exp/chain/tdnn/graph/HCLG.fst",
      "lattice_beam": 3.0,
      "lattice_wspecifier": "ark:/dev/null",
      "max_active": 200,   // the bigger, the better result, the slower
      "nnet3_in": "/usr/local/kaldi/egs/cvte/s5/exp/chain/tdnn/final.mdl",
      "num_threads_startup": 4,  // maybe set to the core number of your CPU.
      "online": false,
      "sample_freq": 16000.0,  
      "word_symbol_table": "/usr/local/kaldi/egs/cvte/s5/exp/chain/tdnn/graph/words.txt"
  }
```

  To run decoder with the configurations above, use: 
  ```
  cd AsrService/Kaldi
  ./kaldi-service ./kaldi_config.json ./decoder_config.json
  ```

## Performance

Well, before I can dig more into Kaldi, the performance is rather poor, see:

(10 parallel requests, localhost, 8 CPUs of Intel(R) Xeon(R) CPU E5-2650 v3 @ 2.30GHz)
```
{
  "Id": "86e96461-0415-4e6f-8bd6-4739858f2ede",
  "Text": "关于黄款汇率希望大家不要被误导当然这个火鸡的答案并不对",
  "Message": "Ok"
}
{
  "Id": "bc44ac47-2d24-4f89-8812-0bc822591091",
  "Text": "关于黄款汇率希望大家不要被误导当然这个火鸡的答案并不对",
  "Message": "Ok"
}
{
  "Id": "323c02cb-3727-44f6-b77b-8237e7615f3c",
  "Text": "关于黄款汇率希望大家不要被误导当然这个火鸡的答案并不对",
  "Message": "Ok"
}
{
  "Id": "2116eb68-47f8-4fa2-bfa4-2766f93426fc",
  "Text": "关于黄款汇率希望大家不要被误导当然这个火鸡的答案并不对",
  "Message": "Ok"
}
{
  "Id": "b77e6d58-be8d-4b6a-b6dd-866a5d9263ef",
  "Text": "关于黄款汇率希望大家不要被误导当然这个火鸡的答案并不对",
  "Message": "Ok"
}
{
  "Id": "e9b13ce1-7f92-4f49-a5be-fc99c4c9a009",
  "Text": "关于黄款汇率希望大家不要被误导当然这个火鸡的答案并不对",
  "Message": "Ok"
}
{
  "Id": "f6f2173e-2814-4af4-8990-84779a145043",
  "Text": "关于黄款汇率希望大家不要被误导当然这个火鸡的答案并不对",
  "Message": "Ok"
}
{
  "Id": "6b239c12-a5dc-47f8-a17a-c31a6e34bb7e",
  "Text": "关于黄款汇率希望大家不要被误导当然这个火鸡的答案并不对",
  "Message": "Ok"
}
{
  "Id": "13ba7bf3-d8e4-421b-8cb8-75172b5f99b8",
  "Text": "关于黄款汇率希望大家不要被误导当然这个火鸡的答案并不对",
  "Message": "Ok"
}
{
  "Id": "fca8ed3c-e4e9-435d-9930-39119529d370",
  "Text": "关于黄款汇率希望大家不要被误导当然这个火鸡的答案并不对",
  "Message": "Ok"
}
avg rtt: 5.460889s 
```
