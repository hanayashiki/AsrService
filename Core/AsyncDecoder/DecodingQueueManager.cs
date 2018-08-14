using log4net;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace Core.AsyncDecoderImpl
{
    class DecodingQueueManager
    {
        private ConcurrentDictionary<string, AutoResetEvent> idToFinishEvent;
        private ConcurrentDictionary<string, DecodeResult> idToResult;

        private readonly ILog _logger;
        public DecodingQueueManager()
        {
            idToFinishEvent = new ConcurrentDictionary<string, AutoResetEvent>();
            idToResult = new ConcurrentDictionary<string, DecodeResult>();

            _logger = LogManager.GetLogger(typeof(DecodingQueueManager));
        }

        public AutoResetEvent RegisterEvent(string id)
        {
            AutoResetEvent finishEvent = new AutoResetEvent(false);
            idToFinishEvent.AddOrUpdate(id, finishEvent, (idx, val) => finishEvent);
            return finishEvent;
        } 

        public DecodeResult PopResult(string id)
        {
            DecodeResult result = null;
            bool success = idToResult.TryRemove(id, out result);
            if (!success)
            {
                _logger.Error($"Strange to find ${id} not found in idToResult.");
                return new DecodeResult { Id = id, Message = "DecodingQueueIdNotFound" };
            }
            return result;
        }

        public Action<DecodeResult> DecodeResultSuccessHandler
        {
            get
            {
                return (result) =>
                {
                    if (result == null)
                    {
                        _logger.Error("null result");
                        return;
                    }
                    if (!idToFinishEvent.ContainsKey(result.Id))
                    {
                        _logger.Error($"{result.Id} not in `idToFinishEvent`");
                        return;
                    }
                    var finishEvent = idToFinishEvent[result.Id];
                    idToResult.AddOrUpdate(result.Id, result, (idx, val) => result);
                    finishEvent.Set();
                };
            }
        }

        public Action<StackExchange.Redis.RedisException> DecodeResultFailureHandler
        {
            get
            {
                return (e) =>
                {
                    _logger.Error(e.Message);
                };
            }
        }
    }
}
