using Core;
using log4net;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
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
        public async Task<string> Decode(IFormFile file)
        {
            Speech speech = new Speech();
            speech.Wave = new byte[file.Length];
            file.CopyTo(new MemoryStream(speech.Wave));
            speech.SpeechId = Convert.ToBase64String(new SHA1CryptoServiceProvider().ComputeHash(speech.Wave));
            DecodeResult result = await _decoder.DecodeAsync(speech);
            return JsonConvert.SerializeObject(result, Formatting.Indented);
        }

    }
}
