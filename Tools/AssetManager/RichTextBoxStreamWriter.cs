// Copyright (c) 2017 Emilian Cioca
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace AssetManager
{
	// Redirects Console output to a textBox control while managing colors.
	public class RichTextBoxStreamWriter : TextWriter
	{
		public static ConsoleColor ForegroundColor = ConsoleColor.Gray;

		static readonly Dictionary<ConsoleColor, Color> colorMap = new Dictionary<ConsoleColor, Color>
		{
			{ ConsoleColor.Black, Color.Black },
			{ ConsoleColor.DarkBlue, Color.DarkBlue },
			{ ConsoleColor.DarkGreen, Color.DarkGreen },
			{ ConsoleColor.DarkCyan, Color.DarkCyan },
			{ ConsoleColor.DarkRed, Color.DarkRed },
			{ ConsoleColor.DarkMagenta, Color.DarkMagenta },
			{ ConsoleColor.DarkYellow, Color.DarkGoldenrod },
			{ ConsoleColor.Gray, Color.Azure },
			{ ConsoleColor.DarkGray, Color.DarkGray },
			{ ConsoleColor.Blue, Color.CornflowerBlue },
			{ ConsoleColor.Green, Color.LightGreen },
			{ ConsoleColor.Cyan, Color.Cyan },
			{ ConsoleColor.Red, Color.Red },
			{ ConsoleColor.Magenta, Color.Magenta },
			{ ConsoleColor.Yellow, Color.Yellow },
			{ ConsoleColor.White, Color.White }
		};

		private readonly RichTextBox output;

		public RichTextBoxStreamWriter(RichTextBox _output)
		{
			output = _output;
		}

		public override void Write(string value)
		{
			Color color;
			colorMap.TryGetValue(ForegroundColor, out color);

			output.SelectionColor = color;

			output.SelectionStart = output.TextLength;
			output.SelectionLength = 0;

			output.AppendText(value);
			output.SelectionColor = output.ForeColor;

			output.ScrollToCaret();
			output.Refresh();
		}

		public override Encoding Encoding
		{
			get { return Encoding.UTF8; }
		}
	}
}
