using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace Mesh_Encoder
{
	public partial class MeshEncoderForm : Form
	{
		public MeshEncoderForm()
		{
			InitializeComponent();
		}

		private void ConvertFile(string file)
		{
			Output.AppendLine(">" + file + " ... ");
			Refresh();

			// Unique data.
			var vertexData = new List<vec3>();
			var textureData = new List<vec2>();
			var normalData = new List<vec3>();
			// Indexing data.
			var faceData = new List<MeshFace>();
			// OpenGL ready data.
			var unPackedData = new List<float>();
			// Keep track of what data is inside the file.
			bool foundUVs = false;
			bool foundNormals = false;

			/* Load .obj file */
			string line = null;
			var reader = new StreamReader(file);
			while ((line = reader.ReadLine()) != null)
			{
				if (line.Contains('#'))
				{
					continue;
				}
				else if (line.Contains("vt "))
				{
					// UVs are in this file.
					foundUVs = true;

					// Load texture coordinates.
					var temp = new vec2();

					var parts = line.Split(' ');
					temp.x = float.Parse(parts[1]);
					temp.y = float.Parse(parts[2]);

					textureData.Add(temp);
				}
				else if (line.Contains("vn "))
				{
					// Normals are in this file.
					foundNormals = true;

					// Load normal.
					var temp = new vec3();

					var parts = line.Split(' ');
					temp.x = float.Parse(parts[1]);
					temp.y = float.Parse(parts[2]);
					temp.z = float.Parse(parts[3]);

					normalData.Add(temp);
				}
				else if (line.Contains("v "))
				{
					// Load vertex.
					var temp = new vec3();

					var parts = line.Split(' ');
					temp.x = float.Parse(parts[1]);
					temp.y = float.Parse(parts[2]);
					temp.z = float.Parse(parts[3]);

					vertexData.Add(temp);
				}
				else if (line.Contains("f "))
				{
					// Load face index.
					var temp = new MeshFace();

					var parts = line.Split(' ');
					var p1 = parts[1].Split('/');
					var p2 = parts[2].Split('/');
					var p3 = parts[3].Split('/');

					temp.vertices[0] = uint.Parse(p1[0]);
					temp.vertices[1] = uint.Parse(p2[0]);
					temp.vertices[2] = uint.Parse(p3[0]);

					if (foundUVs && CheckBox_UV.Checked)
					{
						temp.textures[0] = uint.Parse(p1[1]);
						temp.textures[1] = uint.Parse(p2[1]);
						temp.textures[2] = uint.Parse(p3[1]);
					}

					if (foundNormals && CheckBox_Normals.Checked)
					{
						temp.normals[0] = uint.Parse(p1[2]);
						temp.normals[1] = uint.Parse(p2[2]);
						temp.normals[2] = uint.Parse(p3[2]);
					}

					faceData.Add(temp);
				}
			}

			reader.Close();

			// Unpack the data.
			foreach (MeshFace face in faceData)
			{
				for (uint j = 0; j < 3; j++)
				{
					unPackedData.Add(vertexData[(int)face.vertices[j] - 1].x);
					unPackedData.Add(vertexData[(int)face.vertices[j] - 1].y);
					unPackedData.Add(vertexData[(int)face.vertices[j] - 1].z);

					if (foundUVs && CheckBox_UV.Checked)
					{
						unPackedData.Add(textureData[(int)face.textures[j] - 1].x);
						unPackedData.Add(textureData[(int)face.textures[j] - 1].y);
					}

					if (foundNormals && CheckBox_Normals.Checked)
					{
						unPackedData.Add(normalData[(int)face.normals[j] - 1].x);
						unPackedData.Add(normalData[(int)face.normals[j] - 1].y);
						unPackedData.Add(normalData[(int)face.normals[j] - 1].z);
					}
				}
			}

			// Create a byte buffer containing our data.
			byte[] data = new byte[unPackedData.Count() * 4];

			// Populate the array.
			int count = 0;
			for (int i = 0, size = unPackedData.Count(); i < size; i++)
			{
				byte[] byteArray = BitConverter.GetBytes(unPackedData[i]);

				data[count++] = byteArray[0];
				data[count++] = byteArray[1];
				data[count++] = byteArray[2];
				data[count++] = byteArray[3];
			}

			/* Write .model file */
			string newFile = file.Substring(0, file.Length - 3) + "model";

			BinaryWriter writer = new BinaryWriter(File.Open(newFile, FileMode.Create));
			writer.Write(faceData.Count());
			writer.Write(foundUVs && CheckBox_UV.Checked);
			writer.Write(foundNormals && CheckBox_Normals.Checked);
			writer.Write(data);
			writer.Close();

			Output.AppendText(" Done.");
			Refresh();
		}

		private void Button_ConvertFile_Click(object sender, EventArgs e)
		{
			// Dialog for selecting mesh files.
			OpenFileDialog dialog = new OpenFileDialog();
			dialog.Filter = "object files (*.obj)|*.obj";
			dialog.FilterIndex = 1;
			dialog.RestoreDirectory = true;

			// Ask the user for a file.
			if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				Output.AppendLine(">Converting...");
				ConvertFile(dialog.FileName);
			}
			else
			{
				Output.AppendLine(">Error selecting file.");
			}
		}

		private void Button_ConvertFolder_Click(object sender, EventArgs e)
		{
			// Dialog for selecting a folder.
			FolderBrowserDialog dialog = new FolderBrowserDialog();

			// Ask the user for a folder.
			if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				int count = 0;

				Output.AppendLine(">Converting folder: " + dialog.SelectedPath);
				string[] files = Directory.GetFiles(dialog.SelectedPath);

				Directory.SetCurrentDirectory(dialog.SelectedPath);

				foreach (string file in files)
				{
					if (file.Contains(".obj"))
					{
						ConvertFile(Path.GetFileName(file));
						count++;
					}
				}

				Output.AppendLine(">" + count + " files converted.");
			}
			else
			{
				Output.AppendLine(">Error selecting folder.");
			}
		}
	}

	public static class WinFormsExtensions
	{
		public static void AppendLine(this TextBox source, string value)
		{
			if (source.Text.Length == 0)
			{
				source.Text = value;
			}
			else
			{
				source.AppendText("\r\n" + value);
			}
		}
	}

	public class vec2
	{
		public float x, y;
	};

	public class vec3
	{
		public float x, y, z;
	};

	public class MeshFace
	{
		public MeshFace()
		{}

		public MeshFace(uint v1, uint v2, uint v3,
			uint t1, uint t2, uint t3,
			uint n1, uint n2, uint n3)
		{
			vertices[0] = v1;
			vertices[1] = v2;
			vertices[2] = v3;

			textures[0] = t1;
			textures[1] = t2;
			textures[2] = t3;

			normals[0] = n1;
			normals[1] = n2;
			normals[2] = n3;
		}

		public uint[] vertices = new uint[3];
		public uint[] textures = new uint[3];
		public uint[] normals = new uint[3];
	}
}
