// Copyright (c) 2017 Emilian Cioca
using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace AssetManager
{
	static class Program
	{
		[DllImport("kernel32")]
		private static extern bool AttachConsole(int dwProcessId);
		const int ATTACH_PARENT_PROCESS = -1;

		[STAThread]
		static void Main()
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);
			var builder = new Manager();

			// If we have arguments, process them then exit.
			if (Environment.GetCommandLineArgs().Length > 1)
			{
				if (!AttachConsole(ATTACH_PARENT_PROCESS))
					throw new SystemException("Could not link output to the calling console window.");

				builder.UpdateWorkspace();

				if (Environment.GetCommandLineArgs().Contains("--pack", StringComparer.InvariantCultureIgnoreCase))
				{
					if (!builder.PackWorkspace())
						Environment.ExitCode = 1;

					builder.SaveFileCache();
				}
			}
			else
			{
				// Otherwise we can start the UI normally and route output to the visual textBox.
				Console.SetOut(new RichTextBoxStreamWriter(builder.GetOutput()));

				Application.Run(builder);
			}
		}
	}
}
