using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace AssetManager
{
	static class Program
	{
		static readonly int ATTACH_PARENT_PROCESS = -1;

		[DllImport("kernel32", SetLastError = true)]
		private static extern bool AttachConsole(int dwProcessId);

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

				if (Environment.GetCommandLineArgs().Contains("--update", StringComparer.InvariantCultureIgnoreCase))
				{
					if (!builder.UpdateWorkspace())
					{
						Environment.ExitCode = 1;
						return;
					}
				}

				if (Environment.GetCommandLineArgs().Contains("--pack", StringComparer.InvariantCultureIgnoreCase))
				{
					if (!builder.PackWorkspace())
					{
						Environment.ExitCode = 1;
						return;
					}
				}

				Environment.ExitCode = 0;
				return;
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
