using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace AssetManager
{
	public partial class EncoderList : Form
	{
		public EncoderList(Dictionary<string, string> list)
		{
			InitializeComponent();
			panel.AutoScroll = true;

			encoderList = list;
			RefreshList();
		}

		// Reorganizes the spacing of the list.
		private void RefreshList()
		{
			entries.Clear();

			foreach (var pair in encoderList)
			{
				AddItem(pair.Key, pair.Value);
			}
		}

		private void AddItem(string ext = "", string app = "")
		{
			var pen = new Point(0, EncoderEntry.ItemHeight * entries.Count);

			entries.Add(new EncoderEntry(panel, pen, ext, app));
		}

		private void buttonAdd_Click(object sender, EventArgs e)
		{
			AddItem();
		}

		Dictionary<string, string> encoderList;
		List<EncoderEntry> entries = new List<EncoderEntry>();
	}
}
