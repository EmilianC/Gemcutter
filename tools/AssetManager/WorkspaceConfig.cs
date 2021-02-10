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
		public string outputDirectory;
		public string excludedExtensions;

		[System.NonSerialized()]
		private string[] _excludedExtensions;

		public List<EncoderLink> encoders = new List<EncoderLink>();

		public void Reset()
		{
			// Setup default encoders.
			encoders.Add(new EncoderLink { extension="ttf", encoder="..\\build\\external\\gemcutter\\tools\\FontEncoder\\Release\\font_encoder.exe" });
			encoders.Add(new EncoderLink { extension="obj", encoder="..\\build\\external\\gemcutter\\tools\\MeshEncoder\\Release\\mesh_encoder.exe" });
			encoders.Add(new EncoderLink { extension="png", encoder="..\\build\\external\\gemcutter\\tools\\TextureEncoder\\Release\\texture_encoder.exe" });
			encoders.Add(new EncoderLink { extension="jpg", encoder="..\\build\\external\\gemcutter\\tools\\TextureEncoder\\Release\\texture_encoder.exe" });
			encoders.Add(new EncoderLink { extension="tga", encoder="..\\build\\external\\gemcutter\\tools\\TextureEncoder\\Release\\texture_encoder.exe" });
			encoders.Add(new EncoderLink { extension="bmp", encoder="..\\build\\external\\gemcutter\\tools\\TextureEncoder\\Release\\texture_encoder.exe" });
			encoders.Add(new EncoderLink { extension="mat", encoder="..\\build\\external\\gemcutter\\tools\\MaterialEncoder\\Release\\material_encoder.exe" });

			outputDirectory = "../Assets";
			excludedExtensions = "";
			_excludedExtensions = new string[0];
		}

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
				config.Reset();
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
