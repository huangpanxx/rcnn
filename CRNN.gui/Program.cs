﻿using CRNNnet;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace CRNN.gui
{
    static class Program
    {
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            MyMain(Environment.GetCommandLineArgs());
        }

        static void MyMain(string[] args)
        {
            Console.Write("MODEL:");
            var filename = Console.ReadLine();
            var json = File.ReadAllText(filename);
            CaptchaEngine engine = new CaptchaEngine();
            engine.LoadModel(json, "predict");
            CaptchaServer server = new CaptchaServer(engine);
            server.Start(8080);
        }
    }
}
