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

		string inputPath;
		string outputPath;

		// Automatically refresh the asset directory if there are any changes.
		FileSystemWatcher watcher;
		// The actual native-code encoders.
		Dictionary<string, Encoder> encoders = new Dictionary<string, Encoder>(StringComparer.InvariantCultureIgnoreCase);

		enum BuildMode
		{
			Manual,
			Auto
		};
		BuildMode buildMode = BuildMode.Manual;

		public Manager()
		{
			InitializeComponent();

			inputPath = Directory.GetCurrentDirectory();
			outputPath = Path.GetFullPath(Directory.GetCurrentDirectory() + Path.DirectorySeparatorChar + config.outputDirectory);

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
				NotifyFilter = NotifyFilters.FileName | NotifyFilters.DirectoryName | NotifyFilters.LastWrite
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
				if (e.Name.EndsWith(".meta", StringComparison.InvariantCultureIgnoreCase))
					return;

				RefreshWorkspaceDispatch();
			};

			watcher.Created += (s, e) =>
			{
				if (e.Name.EndsWith(".meta", StringComparison.InvariantCultureIgnoreCase))
					return;

				RefreshWorkspaceDispatch();
				LoadEncoders();

				UpdateFileDispatch(e.FullPath);
			};

			watcher.Changed += (s, e) =>
			{
				if (buildMode == BuildMode.Manual)
					return;

				// Filter out folder names.
				if (!File.Exists(e.FullPath))
					return;

				LoadEncoders();
				if (e.FullPath.EndsWith(".meta", StringComparison.InvariantCultureIgnoreCase))
				{
					PackFileDispatch(e.FullPath.Substring(0, e.FullPath.Length - 5));
				}
				else
				{
					PackFileDispatch(e.FullPath);
				}
			};

			// Populate the workspace for the first time.
			RefreshWorkspace();
		}

		private void PackFile(string file)
		{
			string logName = file.Substring(inputPath.Length + 1);
			string extension = Path.GetExtension(file).Substring(1);

			if (encoders.ContainsKey(extension))
			{
				Log($"Encoding: {logName}");

				var outDir = Path.GetDirectoryName(file.Replace(inputPath, outputPath)) + Path.DirectorySeparatorChar;

				if (!encoders[extension].Convert(file, outDir))
					throw new Exception($"Failed to encode: {logName}");
			}
			else
			{
				var outFile = file.Replace(inputPath, outputPath);

				// Don't bother copying the file if it already exists, unchanged, in the destination folder.
				if (File.Exists(outFile))
				{
					var destinationFileInfo = new FileInfo(outFile);
					var sourceFileInfo = new FileInfo(file);

					if (destinationFileInfo.Length == sourceFileInfo.Length &&
						File.GetLastWriteTime(outFile) == File.GetLastWriteTime(file))
						return;

					File.Delete(outFile);
				}

				Log($"Copying:  {logName}");
				File.Copy(file, outFile);
			}
		}

		private void UpdateFile(string file)
		{
			string extension = Path.GetExtension(file).Substring(1);

			if (encoders.ContainsKey(extension))
			{
				string logName = file.Substring(inputPath.Length + 1);
				Log($"Checking: {logName}");

				if (!encoders[extension].Update(file))
					throw new Exception($"Failed to update: {logName}");
			}
		}

		// Starts the updating process.
		// Any assets without a .meta file will have one added.
		// Any existing .meta files will be updated to the newest versions.
		public void UpdateWorkspace()
		{
			LoadEncoders();
			Icon = Properties.Resources.Building;
			Log(">>>>>> Validating Workspace <<<<<<");

			try
			{
				foreach (var file in GetWorkspaceFiles())
					UpdateFile(file);

				Log(">>>>>> Finished Validating <<<<<<", ConsoleColor.Green);
			}
			catch (Exception e)
			{
				Log($"ERROR:    {e.Message}", ConsoleColor.Red);
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
			LoadEncoders();
			Icon = Properties.Resources.Building;
			Log(">>>>>> Packing Workspace <<<<<<");
			Log($"Output Directory: {outputPath}");

			ButtonPack.Enabled = false;
			ButtonMode.Enabled = false;
			ButtonSettings.Enabled = false;

			// Duplicate the directory structure in the output folder.
			Directory.CreateDirectory(outputPath);
			foreach (var path in Directory.GetDirectories(inputPath, "*", SearchOption.AllDirectories))
			{
				Directory.CreateDirectory(path.Replace(inputPath, outputPath));
			}

			bool result = false;
			var workspaceFiles = GetWorkspaceFiles();
			workspaceFiles.RemoveAll(x => x.EndsWith(".meta", StringComparison.InvariantCultureIgnoreCase));
			try
			{
				foreach (string file in workspaceFiles)
					PackFile(file);

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
				ButtonMode.Enabled = true;
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

		delegate void PackFileDelegate(string file);
		void PackFileDispatch(string file)
		{
			if (treeViewAssets.InvokeRequired)
				Invoke(new PackFileDelegate(PackFile), file);
			else
				PackFile(file);
		}

		delegate void UpdateFileDelegate(string file);
		void UpdateFileDispatch(string file)
		{
			if (treeViewAssets.InvokeRequired)
				Invoke(new UpdateFileDelegate(UpdateFile), file);
			else
				UpdateFile(file);
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
			if (file == null)
				return;

			try
			{
				Process.Start(file);
			}
			catch (System.ComponentModel.Win32Exception)
			{} // The file might not have an associated program to open with.
			catch (FileNotFoundException)
			{}
		}

		private void SetBuildMode(BuildMode mode)
		{
			switch (mode)
			{
			case BuildMode.Auto:
				ButtonMode.Text = "Mode: Auto";
				ButtonPack.Enabled = false;
				break;

			case BuildMode.Manual:
				ButtonMode.Text = "Mode: Manual";
				ButtonPack.Enabled = true;
				break;
			}

			buildMode = mode;
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
				outputPath = Path.GetFullPath(Directory.GetCurrentDirectory() + Path.DirectorySeparatorChar + config.outputDirectory);

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

		private void ButtonMode_Click(object sender, EventArgs e)
		{
			switch (buildMode)
			{
			case BuildMode.Manual:
				SetBuildMode(BuildMode.Auto);
				break;

			case BuildMode.Auto:
				SetBuildMode(BuildMode.Manual);
				break;
			}
		}
	}
}
