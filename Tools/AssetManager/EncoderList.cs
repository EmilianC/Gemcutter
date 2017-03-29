using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Xml.Serialization;

namespace AssetManager
{
	public partial class EncoderList : Form
	{
		public EncoderList(Dictionary<string, string> list)
		{
			InitializeComponent();
			panel.AutoScroll = true;
			encoderList = list;

			foreach (var entry in list)
			{
				AddItem(entry.Key, entry.Value);
			}

			FormClosed += (sender, e) => UpdateList();
		}

		void LoadList()
		{
			//MySerializableClass myObject;

			//// Construct an instance of the XmlSerializer with the type  
			//// of object that is being deserialized.  
			//XmlSerializer mySerializer =
			//new XmlSerializer(typeof(MySerializableClass));

			//// To read the file, create a FileStream.  
			//FileStream myFileStream =
			//new FileStream("myFileName.xml", FileMode.Open);

			//// Call the Deserialize method and cast to the object type.  
			//myObject = (MySerializableClass)
			//mySerializer.Deserialize(myFileStream)
		}

		void SaveList()
		{
			//Employees Emps = new Employees();
			// Note that only the collection is serialized -- not the 
			// CollectionName or any other public property of the class.
			//Emps.CollectionName = "Employees";
			//Employee John100 = new Employee("John", "100xxx");
			//Emps.Add(John100);
			//XmlSerializer x = new XmlSerializer(typeof(Employees));
			//TextWriter writer = new StreamWriter(filename);
			//x.Serialize(writer, Emps);
		}

		// Ensures that the attached list is up to date with the currently valid entries.
		public void UpdateList()
		{
			encoderList.Clear();

			foreach (var entry in entries)
			{
				if (entry.Validate())
				{
					encoderList.Add(entry.extension, entry.encoder);
				}
			}
		}

		// Reorganizes the spacing of the list.
		private void RepositionList()
		{
			for (int i = 0; i < entries.Count; i++)
			{
				entries[i].SetPosition(EncoderEntry.ItemHeight * i);
			}
		}

		private void AddItem(string ext = "", string app = "")
		{
			var entry = new EncoderEntry(panel, EncoderEntry.ItemHeight * entries.Count, ext, app);
			entries.Add(entry);

			entry.onDelete += (sender, e) =>
			{
				entries.Remove(sender as EncoderEntry);
				RepositionList();
			};
		}

		private void buttonAdd_Click(object sender, EventArgs e)
		{
			AddItem();
		}

		Dictionary<string, string> encoderList;
		List<EncoderEntry> entries = new List<EncoderEntry>();
	}
}
