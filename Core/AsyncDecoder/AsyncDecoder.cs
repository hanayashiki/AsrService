using System;
using System.Collections.Concurrent;
using System.Linq;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using log4net;

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
    public class AsyncDecoder : IDecoder
    {
        private DecodingQueueManager queue;
        private RedisWrapper redis;

        private static ConcurrentQueue<long> timeCostRecord = new ConcurrentQueue<long>();
        private readonly ILog _logger;

        // TODO: should be in config
        private static int decodingTimeOut = 3000;
        public AsyncDecoder(bool clear = true)
        {
            queue = new DecodingQueueManager();
            redis = RedisWrapper.Instance;
            redis.RegisterTextSignalHandler(queue.DecodeResultSuccessHandler, queue.DecodeResultFailureHandler);

            _logger = LogManager.GetLogger(typeof(AsyncDecoder));
            if (clear)
            {
                _logger.Info("Redis FLUSHALL");
                redis.FlushAll();
            }
        }

        public async Task<DecodeResult> DecodeAsync(Speech speech)
        {
            Stopwatch sw = new Stopwatch();
            sw.Restart();

            DecodeResult decodeResult;
            try
            {
                redis.Push(speech);
                AutoResetEvent finishEvent = queue.RegisterEvent(speech.SpeechId);
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
            } catch(StackExchange.Redis.RedisTimeoutException e)
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
