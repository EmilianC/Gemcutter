// Copyright (c) 2017 Emilian Cioca
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace AssetManager
{
	public partial class Manager : Form
	{
		// Automatically refresh the asset directory if there are any changes.
		FileSystemWatcher watcher;
		// The form responsible for editing the list of encoders.
		EncoderList encoderForm = new EncoderList();
		// The actual native-code encoders.
		Dictionary<string, Encoder> encoders = new Dictionary<string, Encoder>();

		public Manager()
		{
			InitializeComponent();

			treeViewAssets.AfterSelect += delegate { RefreshItemView(); };
			listBoxAssets.MouseDoubleClick += delegate { OpenAsset(); };
			treeViewAssets.AfterSelect += delegate { buttonAssetOpen.Enabled = true; };
			encoderForm.FormClosing += delegate { LoadEncoders(); };

			// Populate the workspace for the first time.
			RefreshWorkspace();

			// Watch the directory for changes and auto-refresh when needed.
			watcher = new FileSystemWatcher(Directory.GetCurrentDirectory());
			watcher.IncludeSubdirectories = true;
			watcher.EnableRaisingEvents = true;
			watcher.NotifyFilter = NotifyFilters.FileName | NotifyFilters.DirectoryName;
			watcher.Changed += delegate { RefreshWorkspaceDispatch(); };
			watcher.Created += delegate { RefreshWorkspaceDispatch(); };
			watcher.Deleted += delegate { RefreshWorkspaceDispatch(); };
		}

		//- Starts the updating process.
		//- Any assets without a .meta file will have one added.
		//- Assets with an existing .meta file will 
		public bool UpdateWorkspace()
		{
			Log(">>>>>> Started Updating <<<<<<");

			buttonClear.Enabled = false;
			ButtonPack.Enabled = false;
			ButtonUpdate.Enabled = false;

			LoadEncoders();

			string inputRoot = Directory.GetCurrentDirectory();

			bool result = false;
			try
			{
				foreach (var file in GetWorkspaceFiles().Where(file =>
					encoders.Any(x => file.EndsWith(x.Key, StringComparison.InvariantCultureIgnoreCase))))
				{
					string logName = file.Substring(inputRoot.Length + 1);
					Log($"Checking [{logName}]");

					if (!encoders[Path.GetExtension(file).Substring(1)].Update(file))
						throw new Exception($"Failed to update [{logName}]");
				}

				Log(">>>>>> Finished Updating <<<<<<", ConsoleColor.Green);
				result = true;
			}
			catch (Exception e)
			{
				Log($"ERROR:   {e.Message}", ConsoleColor.Red);
				result = false;
			}
			finally
			{
				buttonClear.Enabled = true;
				ButtonPack.Enabled = true;
				ButtonUpdate.Enabled = true;
			}

			return result;
		}

		//- Starts the packing process. Rebuilds the target Asset directory from scratch.
		//- Convertible files are put through the appropriate binary converter. Other files are copied as-is.
		public bool PackWorkspace()
		{
			Log(">>>>>> Started Packing <<<<<<");

			buttonClear.Enabled = false;
			ButtonPack.Enabled = false;
			ButtonUpdate.Enabled = false;

			LoadEncoders();

			string inputRoot = Directory.GetCurrentDirectory();
			string commonRoot = inputRoot.Remove(inputRoot.LastIndexOf(Path.DirectorySeparatorChar));
			string outputRoot = commonRoot + Path.DirectorySeparatorChar + "Assets";

			// Clear and duplicate the directory structure in the output folder.
			if (Directory.Exists(outputRoot))
				Directory.Delete(outputRoot, true);

			Directory.CreateDirectory(outputRoot);

			foreach (var path in Directory.GetDirectories(inputRoot, "*", SearchOption.AllDirectories))
				Directory.CreateDirectory(path.Replace(inputRoot, outputRoot));

			bool result = false;
			var workspaceFiles = GetWorkspaceFiles();
			workspaceFiles.RemoveAll(x => x.EndsWith(".meta", StringComparison.InvariantCultureIgnoreCase));
			try
			{
				// Pack all files with an associated encoder.
				foreach (string file in workspaceFiles.Where(file =>
					encoders.Any(x => file.EndsWith(x.Key, StringComparison.InvariantCultureIgnoreCase))))
				{
					string logName = file.Substring(inputRoot.Length + 1);
					Log($"Encoding [{logName}]");

					var outDir = Path.GetDirectoryName(file.Replace(inputRoot, outputRoot)) + Path.DirectorySeparatorChar;

					if (!encoders[Path.GetExtension(file).Substring(1)].Convert(file, outDir))
						throw new Exception($"Failed to encode [{logName}]");
				}

				// Copy all the remaining files.
				foreach (string file in workspaceFiles.Where(file =>
					!encoders.Any(x => file.EndsWith(x.Key, StringComparison.InvariantCultureIgnoreCase))))
				{
					Log($"Copying  [{file.Substring(inputRoot.Length + 1)}]");

					var outFile = file.Replace(inputRoot, outputRoot);
					File.Copy(file, outFile);
				}

				Log(">>>>>> Finished Packing <<<<<<", ConsoleColor.Green);
				result = true;
			}
			catch (Exception e)
			{
				Log($"ERROR:   {e.Message}", ConsoleColor.Red);
				result = false;
			}
			finally
			{
				buttonClear.Enabled = true;
				ButtonPack.Enabled = true;
				ButtonUpdate.Enabled = true;
			}

			return result;
		}

		void RefreshItemView()
		{
			listBoxAssets.Items.Clear();

			var node = treeViewAssets.SelectedNode;
			if (node == null)
				return;

			foreach (string file in Directory.GetFiles(string.Join("\\", GetNodePath(node))))
				listBoxAssets.Items.Add(Path.GetFileName(file));
		}

		void RefreshWorkspace()
		{
			var previousSelection = GetNodePath(treeViewAssets.SelectedNode);
			treeViewAssets.Nodes.Clear();

			ParseDirectory(Directory.GetCurrentDirectory());

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

			treeViewAssets.ExpandAll();
			RefreshItemView();
		}

		delegate void RefreshWorkspaceDelegate();
		void RefreshWorkspaceDispatch()
		{
			if (treeViewAssets.InvokeRequired)
				Invoke(new RefreshWorkspaceDelegate(RefreshWorkspace));
			else
				RefreshWorkspace();
		}

		// Populates a tree with the contents of a directory on the disk.
		void ParseDirectory(string rootFolder, TreeNode root = null)
		{
			var directories = Directory.GetDirectories(rootFolder);
			Array.Sort(directories);

			foreach (string folder in directories)
			{
				// Add the file count to the name.
				var fileCount = Directory.GetFiles(folder).Where(x => Path.GetExtension(x) != ".meta").Count();
				string name = folder.Split('\\').Last();
				string text = fileCount == 0 ? name : name + " [" + fileCount.ToString() + "]";

				// Append the new node.
				var nodes = root?.Nodes ?? treeViewAssets.Nodes;
				var node = nodes.Add(name, text);

				// Recurse.
				ParseDirectory(folder, node);
			}
		}

		// Returns a list of all the files in the workspace.
		List<string> GetWorkspaceFiles()
		{
			var result = new List<string>();

			foreach (var dir in Directory.GetDirectories(Directory.GetCurrentDirectory(), "*", SearchOption.TopDirectoryOnly))
			{
				foreach (string file in Directory.GetFiles(dir, "*.*", SearchOption.AllDirectories))
					result.Add(file);
			}

			return result;
		}

		// Build the full folder path from the folder tree.
		List<string> GetNodePath(TreeNode node)
		{
			var path = new List<string>();
			while (node != null)
			{
				path.Add(node.Name);
				node = node.Parent;
			}
			path.Reverse();

			return path;
		}

		public RichTextBox GetOutput()
		{
			return Output;
		}

		private void LoadEncoders()
		{
			encoders.Clear();

			try
			{
				foreach (var dll in encoderForm.GetEncoders())
					encoders.Add(dll.Key, new Encoder(dll.Value));
			}
			catch (ArgumentException e)
			{
				Log(e.Message, ConsoleColor.Red);
				encoders.Clear();
			}
		}

		private void Log(string text, ConsoleColor color = ConsoleColor.Gray)
		{
			Console.ForegroundColor = color;
			RichTextBoxStreamWriter.ForegroundColor = color;

			Console.Write(text + "\n");

			Console.ResetColor();
			RichTextBoxStreamWriter.ForegroundColor = ConsoleColor.Gray;
		}

		// Opens the selected asset with the default associated program.
		void OpenAsset()
		{
			var selection = listBoxAssets.SelectedItem;
			if (selection == null)
				return;

			var node = treeViewAssets.SelectedNode;
			if (node == null)
				return;

			var file = string.Join("\\", GetNodePath(node)) + "\\" + listBoxAssets.GetItemText(selection);

			try
			{
				System.Diagnostics.Process.Start(file);
			}
			catch (System.ComponentModel.Win32Exception)
			{
				// The file might not have an associated program to open with.
			}
		}

		private void buttonSettings_Click(object sender, EventArgs e)
		{
			encoderForm.Show();
			encoderForm.BringToFront();
		}

		// Browses to the root workspace directory.
		private void buttonWorkspaceOpen_Click(object sender, EventArgs e)
		{
			System.Diagnostics.Process.Start("explorer.exe", Directory.GetCurrentDirectory());
		}

		// Browses to the selected folder directory.
		private void buttonAssetOpen_Click(object sender, EventArgs e)
		{
			var path = GetNodePath(treeViewAssets.SelectedNode);

			if (path.Count != 0)
				System.Diagnostics.Process.Start("explorer.exe", string.Join("\\", path));
		}

		private void buttonClear_Click(object sender, EventArgs e)
		{
			Output.Clear();
		}

		private void ButtonUpdate_Click(object sender, EventArgs e)
		{
			UpdateWorkspace();
		}

		private void ButtonPack_Click(object sender, EventArgs e)
		{
			PackWorkspace();
		}
	}
}
