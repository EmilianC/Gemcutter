// Copyright (c) 2017 Emilian Cioca
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace AssetManager
{
	public partial class Manager : Form
	{
		// Automatically refresh the asset directory if there are any changes.
		FileSystemWatcher watcher;
		// The actual native-code encoders.
		Dictionary<string, Encoder> encoders = new Dictionary<string, Encoder>();

		public Manager()
		{
			InitializeComponent();

			treeViewAssets.AfterSelect += delegate { buttonAssetOpen.Enabled = true; };

			// Populate the workspace for the first time.
			RefreshWorkspace();

			treeViewAssets.DoubleClick += delegate { OpenAsset(); };

			// Watch the directory for changes and auto-refresh when needed.
			watcher = new FileSystemWatcher(Directory.GetCurrentDirectory());
			watcher.IncludeSubdirectories = true;
			watcher.EnableRaisingEvents = true;
			watcher.NotifyFilter = NotifyFilters.FileName | NotifyFilters.DirectoryName;

			watcher.Changed += (s, e) =>
			{
				RefreshWorkspaceDispatch();
				LoadEncoders();

				if (encoders.Any(x => e.FullPath.EndsWith(x.Key)))
				{
					//pack
				}
				else
				{
					// copy
				}
			};

			watcher.Created += (s, e) =>
			{
				RefreshWorkspaceDispatch();
				LoadEncoders();

				if (encoders.Any(x => e.FullPath.EndsWith(x.Key)))
				{
					encoders[Path.GetExtension(e.FullPath).Substring(1)].Update(e.FullPath);

					// Pack the file as well
				}
			};

			watcher.Deleted += (s, e) =>
			{
				RefreshWorkspaceDispatch();
				LoadEncoders();

				if (encoders.Any(x => e.FullPath.EndsWith(x.Key)) && File.Exists(e.FullPath + ".meta"))
				{
					File.Delete(e.FullPath + ".meta");
				}
			};

			//var cm = new ContextMenu();
			//cm.MenuItems.Add("Clear Console", new EventHandler(buttonClear_Click));
			//Output.ContextMenu = cm;
		}

		//- Starts the updating process.
		//- Any assets without a .meta file will have one added.
		public bool UpdateWorkspace()
		{
			Log(">>>>>> Started Updating <<<<<<");

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

			ButtonPack.Enabled = false;
			ButtonUpdate.Enabled = false;

			LoadEncoders();

			string inputRoot = Directory.GetCurrentDirectory();
			string commonRoot = inputRoot.Remove(inputRoot.LastIndexOf(Path.DirectorySeparatorChar));
			string outputRoot = commonRoot + Path.DirectorySeparatorChar + "Assets";

			// Clear and duplicate the directory structure in the output folder.
			if (Directory.Exists(outputRoot))
			{
				Directory.Delete(outputRoot, true);

				// Wait for deletion.
				var watch = new Stopwatch();
				watch.Start();
				while (Directory.Exists(outputRoot) && watch.Elapsed < TimeSpan.FromSeconds(2)) ;

				if (Directory.Exists(outputRoot))
				{
					Log($"ERROR:   Failed to delete output folder.", ConsoleColor.Red);
					return false;
				}
			}

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
				ButtonPack.Enabled = true;
				ButtonUpdate.Enabled = true;
			}

			return result;
		}

		void RefreshWorkspace()
		{
			var previousSelection = GetNodePath(treeViewAssets.SelectedNode);
			treeViewAssets.Nodes.Clear();

			ParseDirectory(Directory.GetCurrentDirectory());

			// Reselect the old folder, if it still exists.
			if (previousSelection.Length != 0)
			{
				var node = treeViewAssets.Nodes.Find(previousSelection[0], false).FirstOrDefault();

				for (int i = 1; i < previousSelection.Length; ++i)
				{
					if (node == null)
						break;

					node = node.Nodes.Find(previousSelection[i], false).FirstOrDefault();
				}

				treeViewAssets.SelectedNode = node;
			}

			treeViewAssets.ExpandAll();
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

			var nodes = root?.Nodes ?? treeViewAssets.Nodes;

			foreach (string folder in directories)
			{
				string name = folder.Split('\\').Last();

				// Append the new node.
				var node = nodes.Add(name, name);

				var fileCount = 0;
				foreach (var file in Directory.GetFiles(folder).Where(x => Path.GetExtension(x) != ".meta"))
				{
					node.Nodes.Add(Path.GetFileName(file), Path.GetFileName(file));
					fileCount++;
				}

				// Build the name of the node including the file count.
				node.Text = $"{name} [{fileCount}]";

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
		string[] GetNodePath(TreeNode node)
		{
			var path = new List<string>();
			while (node != null)
			{
				path.Add(node.Name);
				node = node.Parent;
			}
			path.Reverse();

			return path.ToArray();
		}

		public RichTextBox GetOutput()
		{
			return Output;
		}

		private void LoadEncoders()
		{
			//encoders.Clear();
			//
			//try
			//{
			//	foreach (var dll in encoderForm.GetEncoders())
			//		encoders.Add(dll.Key, new Encoder(dll.Value));
			//}
			//catch (ArgumentException e)
			//{
			//	Log(e.Message, ConsoleColor.Red);
			//	encoders.Clear();
			//}
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
			var node = treeViewAssets.SelectedNode;
			if (node == null)
				return;

			var file = string.Join("\\", GetNodePath(node));
			if (!File.Exists(file))
				return;

			try
			{
				Process.Start(file);
			}
			catch (System.ComponentModel.Win32Exception)
			{
				// The file might not have an associated program to open with.
			}
		}

		private void buttonSettings_Click(object sender, EventArgs e)
		{
			ButtonPack.Enabled = false;
			ButtonUpdate.Enabled = false;

			var settingsForm = new Settings();
			settingsForm.Show();
			settingsForm.BringToFront();
			settingsForm.FormClosed += delegate 
			{
				ButtonPack.Enabled = true;
				ButtonUpdate.Enabled = true;
			};
		}

		// Browses to the root workspace directory.
		private void buttonWorkspaceOpen_Click(object sender, EventArgs e)
		{
			Process.Start("explorer.exe", Directory.GetCurrentDirectory());
		}

		// Browses to the selected folder directory.
		private void buttonAssetOpen_Click(object sender, EventArgs e)
		{
			var path = GetNodePath(treeViewAssets.SelectedNode);

			if (path.Length != 0)
				Process.Start("explorer.exe", string.Join("\\", path));
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
