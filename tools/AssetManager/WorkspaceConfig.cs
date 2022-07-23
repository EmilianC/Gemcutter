// Copyright (c) 2017 Emilian Cioca
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml.Serialization;

namespace AssetManager
{
	public class EncoderLink
	{
		public string extension;
		public string encoder;
	}

	// Manages the input and output of the workspace settings.
	public class WorkspaceConfig
	{
		public string outputDirectory = "../Assets";
		public string excludedExtensions = "";

		[System.NonSerialized()]
		private string[] _excludedExtensions = new string[0];

		public List<EncoderLink> encoders = new List<EncoderLink>();

		public bool IsExtensionExcluded(string extension)
		{
			return _excludedExtensions.Contains(extension);
		}

		public static WorkspaceConfig Load()
		{
			WorkspaceConfig config;

			try
			{
				using (var myFileStream = File.OpenRead("config.workspace"))
				{
					var mySerializer = new XmlSerializer(typeof(WorkspaceConfig));
					config = (WorkspaceConfig)mySerializer.Deserialize(myFileStream);
				}

				config._excludedExtensions = config.excludedExtensions.Split(';');
			}
			catch (System.Exception)
			{
				config = new WorkspaceConfig();
			}

			return config;
		}

		public void Save()
		{
			if (File.Exists("config.workspace"))
				File.Delete("config.workspace");

			var serializer = new XmlSerializer(typeof(WorkspaceConfig));
			using (var stream = File.OpenWrite("config.workspace"))
			{
				serializer.Serialize(stream, this);
			}
		}
	}
}
