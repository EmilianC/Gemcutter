// Copyright (c) 2017 Emilian Cioca
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using System.Xml.Serialization;

namespace AssetManager
{
	public partial class EncoderList : Form
	{
		List<EncoderEntry> entries = new List<EncoderEntry>();

		public EncoderList()
		{
			InitializeComponent();
			panel.AutoScroll = true;

			LoadEncoders();

			// Hide the form instead of closing it.
			FormClosing += (sender, e) =>
			{
				e.Cancel = true;

				Hide();
				SaveEncoders();
			};
		}

		private void LoadEncoders()
		{
			if (File.Exists("config.workspace"))
			{
				EntryData[] dict;

				using (var myFileStream = File.OpenRead("config.workspace"))
				{
					var mySerializer = new XmlSerializer(typeof(EntryData[]));
					dict = (EntryData[])mySerializer.Deserialize(myFileStream);
				}

				foreach (var entry in dict)
				{
					AddItem(entry.extension, entry.encoder);
				}
			}
			else
			{
				// Setup default encoders.
				entries.Add(new EncoderEntry(panel, EncoderEntry.ItemHeight * 0, 
					"ttf", "%JEWEL3D_PATH%\\Tools\\FontEncoder\\bin\\ReleaseWithExceptions_Win32\\FontEncoder.dll"));
				entries.Add(new EncoderEntry(panel, EncoderEntry.ItemHeight * 1,
					"obj", "%JEWEL3D_PATH%\\Tools\\MeshEncoder\\bin\\ReleaseWithExceptions_Win32\\MeshEncoder.dll"));
			}
		}

		private void SaveEncoders()
		{
			File.WriteAllText("config.workspace", string.Empty);

			using (var writer = File.OpenWrite("config.workspace"))
			{
				var mySerializer = new XmlSerializer(typeof(EntryData[]));
				mySerializer.Serialize(writer, entries.Select(x => new EntryData() {extension = x.extension, encoder = x.encoder }).ToArray());
			}
		}

		// Ensures that the attached list is up to date with the currently valid entries.
		public Dictionary<string, string> GetEncoders()
		{
			var result = new Dictionary<string, string>();

			foreach (var entry in entries)
			{
				if (entry.Validate())
					result.Add(entry.extension, Environment.ExpandEnvironmentVariables(entry.encoder));
			}

			return result;
		}

		// Reorganizes the spacing of the list.
		private void RepositionList()
		{
			for (int i = 0; i < entries.Count; i++)
			{
				entries[i].SetPosition(EncoderEntry.ItemHeight * i);
			}
		}

		private void AddItem(string ext = "", string app = "")
		{
			var entry = new EncoderEntry(panel, EncoderEntry.ItemHeight * entries.Count, ext, app);
			entries.Add(entry);

			entry.onDelete += (sender, e) =>
			{
				entries.Remove(sender as EncoderEntry);
				RepositionList();
			};
		}

		private void buttonAdd_Click(object sender, EventArgs e)
		{
			AddItem();
		}
	}
}
