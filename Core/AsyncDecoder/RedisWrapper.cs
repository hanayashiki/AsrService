using System;
using System.Collections.Generic;
using System.Text;

using StackExchange.Redis;
using MongoDB.Bson;
using MongoDB.Bson.Serialization;
using System.Threading.Tasks;
using log4net;
using Newtonsoft.Json;

namespace Core.AsyncDecoderImpl
{
    public class RedisConfig
    {
        public string IP { set; get; }
        public int Port { get; set; }
    }

    /* 解决和 Redis 的通信问题，包括将数据结构序列化（目前希望用 BSON）
     * 只需要调用 RedisWrapper 就可以传输序列的数据到队列里面 
     * */
    public class RedisWrapper
    {
        private readonly ConnectionMultiplexer redis;
        private readonly IDatabase db;
        private readonly ISubscriber sub;

        /* this is for naming redis message queue */
        private readonly string speechQueue = "SpeechQueue";
        private readonly string textQueue = "TextQueue";
        /* signal channel for more work 
         * https://stackexchange.github.io/StackExchange.Redis/PipelinesMultiplexers */
        private readonly string speechSignal = "SpeechSignal";
        private readonly string textSignal = "TextSignal";

        private readonly static Lazy<RedisWrapper> lazyInstance = new Lazy<RedisWrapper>(() => new RedisWrapper());

        public static RedisWrapper Instance { get => lazyInstance.Value; }

        private readonly ILog _logger = LogManager.GetLogger(typeof(RedisWrapper));
        public static RedisConfig config { set; get; } 

        private RedisWrapper()
        {
            // redis = ConnectionMultiplexer.Connect("localhost");
            _logger.Info("Using configuration: " + JsonConvert.SerializeObject(config));
            redis = ConnectionMultiplexer.Connect(config.IP + ":" + config.Port);
            db = redis.GetDatabase();
            sub = redis.GetSubscriber();
        }
        
        public void FlushAll()
        {
            db.Execute("FLUSHALL");
        }

        public void Push(Speech speech)
        {
            byte[] serialized = speech.ToBson();
            db.ListLeftPush(speechQueue, serialized);
            sub.Publish(speechSignal, "");
        }

        public async Task<DecodeResult> PopDecodeResultAsync()
        {
            RedisValue result = await db.ListRightPopAsync(textQueue);
            RawBsonDocument doc = new RawBsonDocument((byte[])result);
            Console.WriteLine(doc);
	    return BsonSerializer.Deserialize<DecodeResult>((byte[])result);
        }

        public void RegisterTextSignalHandler(Action<DecodeResult> succeedHandler, Action<RedisException> failureHandler)
        {
            sub.Subscribe(textSignal, async (channel, value) =>
            {
                try
                {
                    DecodeResult result = await PopDecodeResultAsync();
                    succeedHandler(result);
                } catch (RedisException e)
                {
                    failureHandler(e);
                }
            });
        }
    }
}
