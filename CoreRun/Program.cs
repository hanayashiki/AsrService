using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;
using Core;
using Core.AsyncDecoderImpl;
using log4net;
using log4net.Config;

namespace CoreRun
{
    class Program
    {
        static async void TestAsync()
        {
            ThreadPool.SetMinThreads(250, 250);

            IDecoder decoder = new AsyncDecoder();
            FakeDecoder.Start();
            byte[] mockWave = new byte[] { 1, 2, 3, 4 };
            IList<Task<DecodeResult>> list = new List<Task<DecodeResult>>();
            for (int i = 0; i < 200; i++) {
                Task<DecodeResult> task = decoder.DecodeAsync(new Speech { SpeechId = i.ToString(), Wave = mockWave });
                //Thread.Sleep(2);
                list.Add(task);
            }
            for (int i = 0; i < 200; i++)
            {
                DecodeResult result = await list[i];
                Console.WriteLine(result.Text + "|" + result.Message);
            }
            
        }

        static async void TestRemote()
        {
            ThreadPool.SetMinThreads(250, 250);

            IDecoder decoder = new AsyncDecoder();

            byte[] mockWave = new byte[] { 1, 2, 3, 4 };
            IList<Task<DecodeResult>> list = new List<Task<DecodeResult>>();
            for (int i = 0; i < 200; i++)
            {
                Task<DecodeResult> task = decoder.DecodeAsync(new Speech { SpeechId = i.ToString(), Wave = mockWave });
                //Thread.Sleep(2);
                list.Add(task);
            }
            for (int i = 0; i < 200; i++)
            {
                DecodeResult result = await list[i];
                Console.WriteLine(result.Text + "|" + result.Message);
            }

        }

        static void Main(string[] args)
        {
            var logCfg = new FileInfo(AppDomain.CurrentDomain.BaseDirectory + "log4net.config");
            XmlConfigurator.ConfigureAndWatch(LogManager.GetRepository(Assembly.GetCallingAssembly()), logCfg);
            TestRemote();
            Console.ReadKey();
        }
    }
}
