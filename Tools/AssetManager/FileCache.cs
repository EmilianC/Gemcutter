// Copyright (c) 2017 Emilian Cioca
using System.Collections.Generic;
using System;
using System.IO;
using System.Xml.Serialization;
using System.Xml.Schema;
using System.Xml;

namespace AssetManager
{
	[XmlRoot("Cache")]
	public class FileCacheDictionary : Dictionary<string, long>, IXmlSerializable
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
				string value = reader.GetAttribute("LastWriteTime");

				long time;
				if (long.TryParse(value, out time))
				{
					Add(key, time);
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
				writer.WriteAttributeString("LastWriteTime", this[key].ToString());
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
			var info = new FileInfo(file);

			if (cache.ContainsKey(file))
			{
				long cacheTime = cache[file];
				if (cacheTime == info.LastWriteTime.Ticks)
				{
					return false;
				}
				else
				{
					cache[file] = info.LastWriteTime.Ticks;
					return true;
				}
			}

			cache.Add(file, info.LastWriteTime.Ticks);
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
	}
}
