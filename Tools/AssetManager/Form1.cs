using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace AssetManager
{
	public partial class formAssetManager : Form
	{
		public formAssetManager()
		{
			InitializeComponent();
			treeViewAssets.AfterSelect += (sender, e) => RefreshItemView();

			RefreshWorkspaceView();

			watcher = new FileSystemWatcher(Directory.GetCurrentDirectory());
			watcher.IncludeSubdirectories = true;
			watcher.EnableRaisingEvents = true;
			watcher.NotifyFilter = NotifyFilters.FileName | NotifyFilters.DirectoryName;
			watcher.Changed += (sender, e) => RefreshWorkspaceViewDispatch();
			watcher.Created += (sender, e) => RefreshWorkspaceViewDispatch();
			watcher.Deleted += (sender, e) => RefreshWorkspaceViewDispatch();
		}

		private void ConvertFile(string file)
		{
			Output.AppendLine(file + " ... ");
			Refresh();

			Output.AppendText(" Done.");
			Refresh();
		}

		private void Button_Pack_Click(object sender, EventArgs e)
		{
			// Ask the user for a folder.
			int count = 0;

			Output.AppendLine("");
			Output.AppendLine("Packing started.");

			// TODO: Recursive search...
			var files = Directory.GetFiles(Directory.GetCurrentDirectory());

			foreach (string file in files)
			{
				if (!file.EndsWith(".meta") && (assetExtensions.Any(x => file.EndsWith(x))))
				{
					ConvertFile(Path.GetFileName(file));
					count++;
				}
			}

			Output.AppendLine(count + " files converted.");
		}

		void RefreshItemView()
		{
			listBoxAssets.Items.Clear();

			var node = treeViewAssets.SelectedNode;
			if (node == null)
				return;

			foreach (string file in Directory.GetFiles(string.Join("\\", GetNodePath(node))))
			{
				listBoxAssets.Items.Add(Path.GetFileName(file));
			}
		}

		void RefreshWorkspaceView()
		{
			var previousSelection = GetNodePath(treeViewAssets.SelectedNode);
			treeViewAssets.Nodes.Clear();

			ParseDirectory(Directory.GetCurrentDirectory(), null);

			treeViewAssets.ExpandAll();

			// Reselect the old folder, if it still exists.
			if (previousSelection.Count != 0)
			{
				var node = treeViewAssets.Nodes.Find(previousSelection[0], false).FirstOrDefault();

				for (int i = 1; i < previousSelection.Count; ++i)
				{
					if (node == null)
						break;

					node = node.Nodes.Find(previousSelection[i], false).FirstOrDefault();
				}

				treeViewAssets.SelectedNode = node;
			}

			RefreshItemView();
		}

		delegate void RefreshWorkspaceViewDelegate();
		void RefreshWorkspaceViewDispatch()
		{
			// Thread-safe dispatch.
			if (treeViewAssets.InvokeRequired)
			{
				Invoke(new RefreshWorkspaceViewDelegate(RefreshWorkspaceView));
			}
			else
			{
				RefreshWorkspaceView();
			}
		}

		void ParseDirectory(string rootFolder, TreeNode root)
		{
			var directories = Directory.GetDirectories(rootFolder);
			Array.Sort(directories);

			foreach (string folder in Directory.GetDirectories(rootFolder))
			{
				TreeNode node;
				string name = folder.Split('\\').Last();

				if (root == null)
					node = treeViewAssets.Nodes.Add(name, name);
				else
					node = root.Nodes.Add(name, name);

				ParseDirectory(folder, node);
			}
		}

		// Build the full folder path from the folder tree.
		List<string> GetNodePath(TreeNode node)
		{
			var path = new List<string>();
			while (node != null)
			{
				path.Add(node.Text);
				node = node.Parent;
			}
			path.Reverse();

			return path;
		}

		private void buttonWorkspaceOpen_Click(object sender, EventArgs e)
		{
			System.Diagnostics.Process.Start("explorer.exe", Directory.GetCurrentDirectory());
		}

		private void buttonAssetOpen_Click(object sender, EventArgs e)
		{
			var path = GetNodePath(treeViewAssets.SelectedNode);

			if (path.Count != 0)
				System.Diagnostics.Process.Start("explorer.exe", string.Join("\\", path));
		}

		// Automatically refresh the asset directory if there are any changes.
		FileSystemWatcher watcher;

		// All convertible file types.
		readonly string[] assetExtensions = { ".obj", ".ttf" };
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
