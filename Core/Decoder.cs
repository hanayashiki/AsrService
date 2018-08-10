using System;
using System.IO;

namespace Core
{
    public interface Decoder
    {
        DecodeResult DecodeAsync(Stream waveStream);
    }
}
