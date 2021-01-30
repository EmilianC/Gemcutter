// Copyright (c) 2017 Emilian Cioca
using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace AssetManager
{
	public partial class Settings : Form
	{
		readonly List<EncoderEntry> entries = new List<EncoderEntry>();

		readonly WorkspaceConfig config;

		public Settings()
		{
			InitializeComponent();
			panel.AutoScroll = true;

			config = WorkspaceConfig.Load();
			TextBoxOutputDirectory.Text = config.outputDirectory;
			TextBoxExcludedExtensions.Text = config.excludedExtensions;
			foreach (var link in config.encoders)
				AddItem(link.extension, link.encoder);

			FormClosing += (_, e) => { config.Save(); };
		}

		// Reorganizes the spacing of the list.
		private void RepositionList()
		{
			for (int i = 0; i < entries.Count; ++i)
			{
				entries[i].SetPosition(EncoderEntry.ItemHeight * i);
			}
		}

		private void AddItem(string ext = "", string app = "")
		{
			var entry = new EncoderEntry(panel, EncoderEntry.ItemHeight * entries.Count, ext, app);
			entries.Add(entry);

			entry.onDelete += (sender, e) => {
				entries.Remove(sender as EncoderEntry);
				RepositionList();
			};
		}

		private void buttonAdd_Click(object sender, EventArgs e)
		{
			AddItem();
		}

		private void TextBoxOutputDirectory_TextChanged(object sender, EventArgs e)
		{
			config.outputDirectory = TextBoxOutputDirectory.Text;
		}

		private void TextBoxExcludedExtensions_TextChanged(object sender, EventArgs e)
		{
			config.excludedExtensions = TextBoxExcludedExtensions.Text;
		}
	}
}
