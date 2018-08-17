using MongoDB.Bson;
using MongoDB.Bson.Serialization;
using MongoDB.Bson.Serialization.Attributes;

using System;
using System.Collections.Generic;
using System.Text;

namespace Core
{
    [BsonNoId]
    public class DecodeResult
    {
       	public string Id { get; set; }
        public string Text { get; set; }
        public string Message { get; set; } = "";
    }
}
