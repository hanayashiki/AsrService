using Core;
using Core.AsyncDecoderImpl;
using MongoDB.Bson;
using MongoDB.Bson.Serialization;
using StackExchange.Redis;
using System;
using System.Collections.Generic;
using System.Text;

namespace Core
{
    public class FakeDecoder
    {
        private readonly ConnectionMultiplexer redis;
        private readonly IDatabase db;
        private readonly ISubscriber sub;

        private uint id = 0;
        public FakeDecoder()
        {
            redis = ConnectionMultiplexer.Connect("localhost");
            db = redis.GetDatabase();
            sub = redis.GetSubscriber();
        }
        public DecodeResult Decode(string speechId)
        {
            return new DecodeResult { Id = speechId, Text = $"Fake decode result {id++}" };
        }
        public static void Start()
        {
            var decoder = new FakeDecoder();

            ISubscriber sub = decoder.redis.GetSubscriber();
            sub.Subscribe("SpeechSignal", (channel, value) =>
            {
                try
                {
                    byte[] work = decoder.db.ListRightPop("SpeechQueue");
                    if (work != null)
                    {
                        Speech speech = BsonSerializer.Deserialize<Speech>(work);
                        DecodeResult result = decoder.Decode(speech.SpeechId);
                        decoder.db.ListLeftPush("TextQueue", result.ToBson());
                        decoder.sub.Publish("TextSignal", "");
                    }
                } catch (RedisTimeoutException)
                {
                    Console.WriteLine("Fakedecoder timeout");
                }
            });
        }

    }
}
