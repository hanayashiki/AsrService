using System;
using System.Collections.Concurrent;
using System.Linq;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using log4net;
using System.IO;
using Newtonsoft.Json;

namespace Core.AsyncDecoderImpl
{
    /* Decode asynchronically: AsyncDecoder sends requests to `RedisWrapper`,
     * and asks `DecodingQueueManager` to wake it up and sleeps.
     * `DecodingQueueManager` wakes up the thread and the decoding result is 
     * stored internally. Then we can pop the result out and return to higher.
     * 
     * Intrinsically, see comments in `RedisWrapper` and `DecodingQueueManager` 
     * 
     * **/

    public class AsyncDecoderConfig
    {
        public bool ClearRedis { get; set; }
        public int DecodingTimeoutMs { get; set; }
        public int MaxRetrials { get; set; }
        public bool EnableFakeDecoder { get; set; }
        public RedisConfig Redis { get; set; }
        public static AsyncDecoderConfig FromFile(string pathToConfig)
        {
            return JsonConvert.DeserializeObject<AsyncDecoderConfig>(File.ReadAllText(pathToConfig));
        }
    }

    public class AsyncDecoder : IDecoder
    {
        private DecodingQueueManager queue;
        private RedisWrapper redis;

        private static ConcurrentQueue<long> timeCostRecord = new ConcurrentQueue<long>();
        private readonly ILog _logger;

        private readonly AsyncDecoderConfig _config;

        // TODO: should be in config
        private static int decodingTimeOut = 3000;
        public AsyncDecoder(AsyncDecoderConfig config)
        {
            _config = config;
            RedisWrapper.config = config.Redis;
            queue = new DecodingQueueManager();
            redis = RedisWrapper.Instance;
            redis.RegisterTextSignalHandler(queue.DecodeResultSuccessHandler, queue.DecodeResultFailureHandler);

            _logger = LogManager.GetLogger(typeof(AsyncDecoder));
            decodingTimeOut = config.DecodingTimeoutMs;
            if (config.ClearRedis)
            {
                _logger.Info("Redis FLUSHALL");
                redis.FlushAll();
            }
            _logger.Info("Using configuration: " + JsonConvert.SerializeObject(config));

            if (config.EnableFakeDecoder)
            {
                FakeDecoder.Start();
            }
        }

        public async Task<DecodeResult> DecodeAsync(Speech speech)
        {
            DecodeResult result;
            int i = 0;
            do
            {
                result = await DecodyAsyncImpl(speech);
                if (result.Message == "Ok")
                {
                    return result;
                } else {
		    // if failed we should give a new Id to indicate different speeches
		    speech.SpeechId = Guid.NewGuid().ToString();
		}
                i++;
            } while (i < _config.MaxRetrials);
            return result;
        }

        private async Task<DecodeResult> DecodyAsyncImpl(Speech speech)
        {
            Stopwatch sw = new Stopwatch();
            sw.Restart();

            DecodeResult decodeResult;
            try
            {
                AutoResetEvent finishEvent = queue.RegisterEvent(speech.SpeechId);
                redis.Push(speech);
		bool success = await Task.Run(() =>
                {
                    bool result = finishEvent.WaitOne(decodingTimeOut);
                    if (!result)
                    {
                        _logger.Error($"Decoding timeout for Speech {speech.SpeechId}");
                        return false;
                    }
                    return true;
                });
                if (!success)
                {
                    return new DecodeResult { Id = speech.SpeechId, Message = "TimeOut" };
                }
                decodeResult = queue.PopResult(speech.SpeechId);
            }
            catch (StackExchange.Redis.RedisTimeoutException e)
            {
                decodeResult = new DecodeResult { Id = speech.SpeechId, Message = "RedisTimeOut" };
                _logger.Fatal(e.Message);
            }

            timeCostRecord.Enqueue(sw.ElapsedMilliseconds);
            if (timeCostRecord.Count > 100) timeCostRecord.TryDequeue(out _);
            _logger.Info($"RTT {speech.SpeechId}: {sw.ElapsedMilliseconds}ms");
            _logger.Info($"Average RTT: {(1.0 * timeCostRecord.Sum() / timeCostRecord.Count)}ms");
            return decodeResult;
        } 
    }
}
