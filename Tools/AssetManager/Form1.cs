using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace AssetManager
{
	public partial class formAssetManager : Form
	{
		public void RefreshAssetList()
		{
			ParseDirectory(Directory.GetCurrentDirectory(), null);
		}

		private void ParseDirectory(string rootFolder, TreeNode root)
		{
			var directories = Directory.GetDirectories(rootFolder);
			Array.Sort(directories);

			foreach (string folder in Directory.GetDirectories(rootFolder))
			{
				TreeNode node;
				if (root == null)
				{
					node = treeViewAssets.Nodes.Add(folder.Split('\\').Last());
				}
				else
				{
					node = root.Nodes.Add(folder.Split('\\').Last());
				}

				ParseDirectory(folder, node);
			}
		}

		public formAssetManager()
		{
			InitializeComponent();
			treeViewAssets.AfterSelect += TreeViewEventHandler;

			RefreshAssetList();

			watcher = new FileSystemWatcher(Directory.GetCurrentDirectory());
			watcher.IncludeSubdirectories = true;
			watcher.Changed += FileSystemEventHandler;
			watcher.Created += FileSystemEventHandler;
			watcher.Deleted += FileSystemEventHandler;

			treeViewAssets.ExpandAll();
		}

		void TreeViewEventHandler(object sender, TreeViewEventArgs e)
		{
			listBoxAssets.Items.Clear();

			// Build the full folder path from the folder tree.
			var path = new List<string>();
			TreeNode node = treeViewAssets.SelectedNode;
			while (node != null)
			{
				path.Add(node.Text);
				node = node.Parent;
			}
			path.Reverse();

			foreach (string file in Directory.GetFiles(string.Join("\\", path)))
			{
				listBoxAssets.Items.Add(Path.GetFileName(file));
			}
		}

		private void ConvertFile(string file)
		{
			Output.AppendLine(file + " ... ");
			Refresh();

			Output.AppendText(" Done.");
			Refresh();
		}

		private void Button_ConvertFolder_Click(object sender, EventArgs e)
		{
			// Ask the user for a folder.
			int count = 0;

			Output.AppendLine("");
			Output.AppendLine("Packing started.");

			// TODO: Recursive search...
			var files = Directory.GetFiles(Directory.GetCurrentDirectory());

			foreach (string file in files)
			{
				if (!file.EndsWith(".meta") && (file.EndsWith(".obj") || file.EndsWith(".ttf")))
				{
					ConvertFile(Path.GetFileName(file));
					count++;
				}
			}

			Output.AppendLine(count + " files converted.");
		}

		// Automatically refresh the asset directory if there are any changes.
		FileSystemWatcher watcher;
		void FileSystemEventHandler(object sender, FileSystemEventArgs e)
		{
			RefreshAssetList();
		}
	}

	public static class WinFormsExtensions
	{
		public static void AppendLine(this TextBox source, string value)
		{
			if (source.Text.Length == 0)
			{
				source.Text = value;
			}
			else
			{
				source.AppendText("\r\n" + value);
			}
		}
	}
}
