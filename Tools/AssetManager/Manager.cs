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
		WorkspaceConfig config = WorkspaceConfig.Load();

		// Automatically refresh the asset directory if there are any changes.
		FileSystemWatcher watcher;
		// The actual native-code encoders.
		Dictionary<string, Encoder> encoders = new Dictionary<string, Encoder>();

		public Manager()
		{
			InitializeComponent();

			buttonEditMetadata.Click += delegate { OpenFile(GetSelectedItem() + ".meta"); };

			treeViewAssets.DoubleClick += delegate { OpenFile(GetSelectedItem()); };

			treeViewAssets.AfterSelect += delegate
			{
				buttonEditMetadata.Enabled = File.Exists(GetSelectedItem() + ".meta");
			};

			// Watch the directory for changes and auto-refresh when needed.
			watcher = new FileSystemWatcher
			{
				Path = Directory.GetCurrentDirectory(),
				IncludeSubdirectories = true,
				EnableRaisingEvents = true,
				NotifyFilter = NotifyFilters.FileName | NotifyFilters.DirectoryName
			};

			watcher.Renamed += (s, e) =>
			{
				RefreshWorkspaceDispatch();

				if (File.Exists(e.OldFullPath + ".meta"))
				{
					File.Move(e.OldFullPath + ".meta", e.FullPath + ".meta");
				}
			};

			watcher.Deleted += (s, e) =>
			{
				if (e.Name.EndsWith(".meta"))
					return;

				RefreshWorkspaceDispatch();
			};

			watcher.Created += (s, e) =>
			{
				if (e.Name.EndsWith(".meta"))
					return;

				RefreshWorkspaceDispatch();

				if (config.encoders.Any(x => e.FullPath.EndsWith(x.extension)))
				{
					LoadEncoders();

					try
					{
						encoders[Path.GetExtension(e.FullPath).Substring(1)].Update(e.FullPath);
					}
					catch (Exception ex)
					{
						Log($"ERROR:    {ex.Message}", ConsoleColor.Red);
					}
				}
			};

			// Populate the workspace for the first time.
			RefreshWorkspace();
		}

		// Starts the updating process.
		// Any assets without a .meta file will have one added.
		// Any existing .meta files will be updated to the newest versions.
		public void UpdateWorkspace()
		{
			Icon = Properties.Resources.Building;

			Log(">>>>>> Validating Workspace <<<<<<");

			LoadEncoders();

			string inputRoot = Directory.GetCurrentDirectory();

			try
			{
				foreach (var file in GetWorkspaceFiles().Where(file =>
					encoders.Any(x => file.EndsWith(x.Key, StringComparison.InvariantCultureIgnoreCase))))
				{
					string logName = file.Substring(inputRoot.Length + 1);
					Log($"Checking: {logName}");

					if (!encoders[Path.GetExtension(file).Substring(1)].Update(file))
						throw new Exception($"Failed to update: {logName}");
				}

				Log(">>>>>> Finished Validating <<<<<<", ConsoleColor.Green);
			}
			catch (Exception e)
			{
				Log($"ERROR:   {e.Message}", ConsoleColor.Red);
			}
			finally
			{
				Icon = Properties.Resources.JewelIcon;
			}
		}

		// Starts the packing process. Rebuilds the target Asset directory from scratch.
		// Convertible files are put through the appropriate binary converter. Other files are copied as-is.
		public bool PackWorkspace()
		{
			Icon = Properties.Resources.Building;

			string inputRoot = Directory.GetCurrentDirectory();
			string outputRoot = Path.GetFullPath(inputRoot + Path.DirectorySeparatorChar + config.outputDirectory);

			Log(">>>>>> Started Packing <<<<<<");
			Log($"Output Directory: {outputRoot}");

			ButtonPack.Enabled = false;
			ButtonSettings.Enabled = false;

			LoadEncoders();

			// Duplicate the directory structure in the output folder.
			Directory.CreateDirectory(outputRoot);
			foreach (var path in Directory.GetDirectories(inputRoot, "*", SearchOption.AllDirectories))
			{
				Directory.CreateDirectory(path.Replace(inputRoot, outputRoot));
			}

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
					Log($"Encoding: {logName}");

					var outDir = Path.GetDirectoryName(file.Replace(inputRoot, outputRoot)) + Path.DirectorySeparatorChar;

					if (!encoders[Path.GetExtension(file).Substring(1)].Convert(file, outDir))
						throw new Exception($"Failed to encode: {logName}");
				}

				// Copy all the remaining files.
				foreach (string file in workspaceFiles.Where(file =>
					!encoders.Any(x => file.EndsWith(x.Key, StringComparison.InvariantCultureIgnoreCase))))
				{
					var outFile = file.Replace(inputRoot, outputRoot);

					// Don't bother copying the file if it already exists, unchanged, in the destination folder.
					if (File.Exists(outFile))
					{
						var destinationFileInfo = new FileInfo(outFile);
						var sourceFileInfo = new FileInfo(file);

						if (destinationFileInfo.Length == sourceFileInfo.Length &&
							File.GetLastWriteTime(outFile) == File.GetLastWriteTime(file))
							continue;

						File.Delete(outFile);
					}

					Log($"Copying:  {file.Substring(inputRoot.Length + 1)}");
					File.Copy(file, outFile);
				}

				Log(">>>>>> Finished Packing <<<<<<", ConsoleColor.Green);
				result = true;
			}
			catch (Exception e)
			{
				Log($"ERROR:    {e.Message}", ConsoleColor.Red);
				result = false;
			}
			finally
			{
				ButtonPack.Enabled = true;
				ButtonSettings.Enabled = true;

				Icon = Properties.Resources.JewelIcon;
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
				var name = Path.GetFileName(folder);
				var folderNode = nodes.Add(name, name);

				// Find and add all files in the folder.
				var fileCount = 0;
				foreach (var file in Directory.GetFiles(folder).Where(x =>
					Path.GetExtension(x) != ".meta" && 
					!config.excludedExtensions.Split(';').Contains(Path.GetExtension(x).Substring(1))))
				{
					var fileName = Path.GetFileName(file);
					folderNode.Nodes.Add(fileName, fileName);
					fileCount++;
				}

				folderNode.Text = $"{name} [{fileCount}]";

				// Recurse.
				ParseDirectory(folder, folderNode);
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
			encoders.Clear();
			
			try
			{
				foreach (var dll in config.encoders)
					encoders.Add(dll.extension, new Encoder(Environment.ExpandEnvironmentVariables(dll.encoder)));
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

		private string GetSelectedItem()
		{
			var node = treeViewAssets.SelectedNode;
			if (node == null)
				return null;

			var file = string.Join(Path.DirectorySeparatorChar.ToString(), GetNodePath(node));

			return file;
		}

		// Opens the selected asset with the default associated program.
		private void OpenFile(string file)
		{
			try
			{
				Process.Start(file);
			}
			catch (System.ComponentModel.Win32Exception)
			{} // The file might not have an associated program to open with.
			catch (FileNotFoundException)
			{}
		}

		private void buttonSettings_Click(object sender, EventArgs e)
		{
			ButtonPack.Enabled = false;
			ButtonSettings.Enabled = false;

			var settingsForm = new Settings();
			settingsForm.Show();
			settingsForm.BringToFront();
			settingsForm.FormClosed += delegate 
			{
				ButtonPack.Enabled = true;
				ButtonSettings.Enabled = true;

				// Refresh settings.
				config = WorkspaceConfig.Load();

				RefreshWorkspace();
			};
		}

		// Browses to the root workspace directory.
		private void buttonWorkspaceOpen_Click(object sender, EventArgs e)
		{
			Process.Start("explorer.exe", Directory.GetCurrentDirectory());
		}

		private void ButtonPack_Click(object sender, EventArgs e)
		{
			PackWorkspace();
		}

		private void buttonExpand_Click(object sender, EventArgs e)
		{
			treeViewAssets.ExpandAll();
		}

		private void buttonCollapse_Click(object sender, EventArgs e)
		{
			treeViewAssets.CollapseAll();
		}
	}
}
