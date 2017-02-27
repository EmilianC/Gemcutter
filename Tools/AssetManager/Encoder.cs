// Copyright (c) 2017 Emilian Cioca
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AssetManager
{
	class Encoder
	{
		public Encoder()
		{
			//...
		}

		delegate void onProcessExit(int exitCode);

		private System.Diagnostics.Process process = new System.Diagnostics.Process();
		private string output;
	}
}
