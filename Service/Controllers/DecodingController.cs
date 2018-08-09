using log4net;
using Microsoft.AspNetCore.Mvc;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Service.Controllers
{
    [Route("[controller]/[action]")]
    public class DecodingController
    {
        private readonly ILog _logger = LogManager.GetLogger(typeof(DecodingController));
        DecodingController()
        {
        }
    }
}
