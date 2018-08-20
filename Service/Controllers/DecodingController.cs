using Core;
using log4net;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace Service.Controllers
{
    [Route("[controller]/[action]")]
    public class DecodingController
    {
        private readonly ILog _logger = LogManager.GetLogger(typeof(DecodingController));
        private readonly IDecoder _decoder;
        public DecodingController(IDecoder decoder)
        {
            _decoder = decoder;
        }
        [HttpPost]
        public async Task<string> Decode(IFormFile wave)
        {
            if (wave == null)
            {
                DecodeResult errorResult = new DecodeResult
                {
                    Id = "",
                    Text = "",
                    Message = "Usage: POST <host_name>/Decoding/Decode\n" +
                                "Content-Disposition: form-data; name=\"wave\"; filename=\"your.wav\"\n" +
                                "Content-Type: audio/x-wav\n\n" +
                                "<binary_contents>"
                };
                return JsonConvert.SerializeObject(errorResult, Formatting.Indented);
            }

            Speech speech = new Speech();
            speech.Wave = new byte[wave.Length];
            wave.CopyTo(new MemoryStream(speech.Wave));
            speech.SpeechId = Guid.NewGuid().ToString();
            DecodeResult result = await _decoder.DecodeAsync(speech);
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

    }
}
