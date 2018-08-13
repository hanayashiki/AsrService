using System;
using System.IO;
using System.Threading.Tasks;

namespace Core
{
    public interface IDecoder
    {
        Task<DecodeResult> DecodeAsync(Speech speech);
    }
}
