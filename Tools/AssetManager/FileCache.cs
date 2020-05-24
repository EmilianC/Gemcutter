// Copyright (c) 2017 Emilian Cioca
using System.Collections.Generic;
using System;
using System.IO;
using System.Xml.Serialization;
using System.Xml.Schema;
using System.Xml;

namespace AssetManager
{
	public struct TimeStamps
	{
		// Last-write time in ticks for the asset.
		public long fileTime;
		// Last-write time in ticks for the metadata file.
		public long metaTime;
	};

	[XmlRoot("Cache")]
	public class FileCacheDictionary : Dictionary<string, TimeStamps>, IXmlSerializable
	{
		public XmlSchema GetSchema() { return null; }

		public void ReadXml(XmlReader reader)
		{
			if (reader.IsEmptyElement)
				return;

			reader.Read();
			while (reader.NodeType != XmlNodeType.EndElement)
			{
				string key = reader.GetAttribute("File");
				string fileTime = reader.GetAttribute("LastFileWriteTime");
				string metaTime = reader.GetAttribute("LastMetaWriteTime");

				TimeStamps times;
				if (long.TryParse(fileTime, out times.fileTime) &&
					long.TryParse(metaTime, out times.metaTime))
				{
					Add(key, times);
				}

				reader.Read();
			}
		}

		public void WriteXml(XmlWriter writer)
		{
			foreach (var key in Keys)
			{
				writer.WriteStartElement("Cache");
				writer.WriteAttributeString("File", key.ToString());
				writer.WriteAttributeString("LastFileWriteTime", this[key].fileTime.ToString());
				writer.WriteAttributeString("LastMetaWriteTime", this[key].metaTime.ToString());
				writer.WriteEndElement();
			}
		}
	}

	// Keeps track of the last time that files were updated.
	public class FileCache
	{
		public FileCacheDictionary cache = new FileCacheDictionary();

		public bool ShouldPack(string file)
		{
			TimeStamps item;
			item.fileTime = GetLastWriteTime(file);
			item.metaTime = GetLastWriteTime(file + ".meta");

			if (cache.ContainsKey(file))
			{
				var cached = cache[file];

				if (item.fileTime == cached.fileTime &&
					item.metaTime == cached.metaTime)
				{
					return false;
				}
				else
				{
					cache[file] = item;
					return true;
				}
			}

			cache.Add(file, item);
			return true;
		}

		public static FileCache Load()
		{
			FileCache config;
			try
			{
				using (var myFileStream = File.OpenRead("Files.cache"))
				{
					var mySerializer = new XmlSerializer(typeof(FileCache));
					config = (FileCache)mySerializer.Deserialize(myFileStream);
				}
			}
			catch (Exception)
			{
				config = new FileCache();
			}

			return config;
		}

		public void Save()
		{
			if (File.Exists("Files.cache"))
				File.Delete("Files.cache");

			var serializer = new XmlSerializer(typeof(FileCache));
			using (var stream = File.OpenWrite("Files.cache"))
			{
				serializer.Serialize(stream, this);
			}
		}

		public void Clear()
		{
			cache.Clear();
		}

		private long GetLastWriteTime(string file)
		{
			if (!File.Exists(file))
				return 0;

			var fileInfo = new FileInfo(file);

			return fileInfo.LastWriteTime.Ticks;
		}
	}
}
