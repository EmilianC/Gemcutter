// Copyright (c) 2017 Emilian Cioca
using System.Collections.Generic;
using System.IO;
using System.Xml.Serialization;

namespace AssetManager
{
	public class EncoderLink
	{
		public string extension;
		public string encoder;
	}

	public class WorkspaceConfig
	{
		public string OutputDirectory;
		public string ExcludedExtensions;
		public List<EncoderLink> encoders = new List<EncoderLink>();

		WorkspaceConfig()
		{
		}

		public void Reset()
		{
			// Setup default encoders.
			encoders.Add(new EncoderLink { extension="ttf", encoder="%JEWEL3D_PATH%\\Tools\\FontEncoder\\bin\\ReleaseWithExceptions_Win32\\FontEncoder.dll" });
			encoders.Add(new EncoderLink { extension="obj", encoder="%JEWEL3D_PATH%\\Tools\\MeshEncoder\\bin\\ReleaseWithExceptions_Win32\\MeshEncoder.dll" });
			encoders.Add(new EncoderLink { extension="png", encoder="%JEWEL3D_PATH%\\Tools\\TextureEncoder\\bin\\ReleaseWithExceptions_Win32\\TextureEncoder.dll"});
			encoders.Add(new EncoderLink { extension="jpg", encoder="%JEWEL3D_PATH%\\Tools\\TextureEncoder\\bin\\ReleaseWithExceptions_Win32\\TextureEncoder.dll"});
			encoders.Add(new EncoderLink { extension="tga", encoder="%JEWEL3D_PATH%\\Tools\\TextureEncoder\\bin\\ReleaseWithExceptions_Win32\\TextureEncoder.dll"});
			encoders.Add(new EncoderLink { extension="bmp", encoder="%JEWEL3D_PATH%\\Tools\\TextureEncoder\\bin\\ReleaseWithExceptions_Win32\\TextureEncoder.dll" });

			OutputDirectory = "../Assets";
			ExcludedExtensions = "";
		}

		static public WorkspaceConfig Load()
		{
			WorkspaceConfig config;
			if (File.Exists("config.workspace"))
			{
				using (var myFileStream = File.OpenRead("config.workspace"))
				{
					var mySerializer = new XmlSerializer(typeof(WorkspaceConfig));
					config = (WorkspaceConfig)mySerializer.Deserialize(myFileStream);
				}
			}
			else
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
