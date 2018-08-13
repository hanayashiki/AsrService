using System;
using MongoDB.Bson;
using MongoDB.Bson.Serialization;
using StackExchange.Redis;

namespace TestRedis
{
    public class DecodeResult
    {
        public string Id { get; set; }
        public string Text { get; set; }
    }
    class Program
    {
        static ConnectionMultiplexer redis = ConnectionMultiplexer.Connect("localhost");
        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");
            IDatabase db = redis.GetDatabase();

            string value = "abcdefg";
            db.StringSet("mykey", value);

            string gotValue = db.StringGet("mykey");
            Console.WriteLine("mykey: " + gotValue);

            db.ListLeftPush("fuckinglist", "val1");
            string listpopped = db.ListLeftPop("fuckinglist");
            Console.WriteLine("fuckinglist head poped: " + listpopped);

            DecodeResult res = new DecodeResult { Id = "Shit", Text = "Fuck" };
            var serializedResult = res.ToBson();
            DecodeResult deserialized = BsonSerializer.Deserialize<DecodeResult>(serializedResult);
            Console.WriteLine(res.Id);
            Console.WriteLine(res.Text);
            Console.WriteLine(deserialized.Id);
            Console.WriteLine(deserialized.Text);

            db.StringSet("fucked", serializedResult);
            byte[] fromRedis = db.StringGet("fucked");
            DecodeResult deserialized2 = BsonSerializer.Deserialize<DecodeResult>(fromRedis);
            Console.WriteLine(deserialized2.Id);
            Console.WriteLine(deserialized2.Text);

            Console.ReadKey();
        }
    }
}
