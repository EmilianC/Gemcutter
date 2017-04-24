// Copyright (c) 2017 Emilian Cioca
using System;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using System.Drawing;
using System.Xml.Serialization;

namespace AssetManager
{
	public class EntryData
	{
		[XmlAttribute]
		public string extension;
		[XmlAttribute]
		public string encoder;
	}

	public class EncoderEntry
	{
		public EncoderEntry(Panel parentPanel, int offset, string ext = "", string app = "")
		{
			parent = parentPanel;
			CreateControls();
			SetPosition(offset);

			extension = ext;
			encoder = app;
		}

		private void DeleteControls()
		{
			status.Dispose();
			extensionBox.Dispose();
			encoderBox.Dispose();
			search.Dispose();
			delete.Dispose();

			status = null;
			extensionBox = null;
			encoderBox = null;
			search = null;
			delete = null;
		}

		public string extension
		{
			get { return extensionBox.Text;  }
			set { extensionBox.Text = value; Validate(); }
		}

		public string encoder
		{
			get { return encoderBox.Text; }
			set { encoderBox.Text = value; Validate(); }
		}

		public void SetPosition(int offset)
		{
			// Adjust for any scrolling offset.
			offset += parent.AutoScrollPosition.Y;

			status.Location = new Point(3, 9 + offset);
			extensionBox.Location = new Point(27, 7 + offset);
			encoderBox.Location = new Point(78, 7 + offset);
			search.Location = new Point(584, 7 + offset);
			delete.Location = new Point(610, 7 + offset);
		}

		// Returns true if the contained data represents a valid extension/encoder pair.
		public bool Validate()
		{
			status.BackgroundImage = Properties.Resources.Error as Image;

			if (extensionBox.Text.Length == 0 ||
				encoderBox.Text.Length == 0)
			{
				statusLog.SetToolTip(status, "Fields cannot be empty.");
				return false;
			}

			if (extensionBox.Text.Any(x => Path.GetInvalidFileNameChars().Contains(x)))
			{
				statusLog.SetToolTip(status, "The Extension name contains invalid characters.");
				return false;
			}

			if (extensionBox.Text.Equals("meta", StringComparison.InvariantCultureIgnoreCase))
			{
				statusLog.SetToolTip(status, "This Extension is reserved in the workspace and cannot be used.");
				return false;
			}

			if (encoderBox.Text.Any(x => Path.GetInvalidPathChars().Contains(x)) ||
				!encoderBox.Text.EndsWith(".dll", System.StringComparison.InvariantCultureIgnoreCase))
			{
				statusLog.SetToolTip(status, "The Encoder is not a valid path to a dynamic library.");
				return false;
			}

			var path = Environment.ExpandEnvironmentVariables(encoderBox.Text);
			var encoderPath = Path.IsPathRooted(path) ?
				path :
				Directory.GetCurrentDirectory() + path;
			if (!File.Exists(encoderPath))
			{
				statusLog.SetToolTip(status, "The Encoder does not exist on the disk.");
				return false;
			}

			status.BackgroundImage = Properties.Resources.Correct as Image;
			statusLog.SetToolTip(status, "Extension / Encoder pair are valid.");
			return true;
		}

		private void CreateControls()
		{
			var font = new Font("Segoe UI", 9F);

			// Manually create the GUI for the table entry.
			status.BackgroundImageLayout = ImageLayout.Stretch;
			status.Size = new Size(18, 18);
			status.TabStop = false;

			extensionBox.Size = new Size(45, 23);
			extensionBox.TabIndex = tabIndex++;
			extensionBox.Font = font;
			extensionBox.TextChanged += delegate { Validate(); };

			encoderBox.Size = new Size(500, 23);
			encoderBox.TabIndex = tabIndex++;
			encoderBox.Font = font;
			encoderBox.TextChanged += delegate { Validate(); };

			search.Size = new Size(23, 23);
			search.BackgroundImage = Properties.Resources.Search;
			search.BackgroundImageLayout = ImageLayout.Stretch;
			search.TabIndex = tabIndex++;
			search.UseVisualStyleBackColor = true;
			search.Click += delegate { SearchForEncoder(); };

			delete.Size = new Size(23, 23);
			delete.BackgroundImage = Properties.Resources.Delete;
			delete.BackgroundImageLayout = ImageLayout.Stretch;
			delete.TabIndex = tabIndex++;
			delete.UseVisualStyleBackColor = true;
			delete.Click += delegate
			{
				DeleteControls();

				if (onDelete != null)
					onDelete(this, EventArgs.Empty);
			};

			parent.Controls.Add(status);
			parent.Controls.Add(extensionBox);
			parent.Controls.Add(encoderBox);
			parent.Controls.Add(search);
			parent.Controls.Add(delete);
		}

		private void SearchForEncoder()
		{
			OpenFileDialog dialog = new OpenFileDialog();
			dialog.Filter = "dynamic libraries (*.dll)|*.dll";
			dialog.FilterIndex = 1;
			dialog.RestoreDirectory = true;

			if (dialog.ShowDialog() == DialogResult.OK)
				encoderBox.Text = dialog.FileName;
		}

		// Called when the delete button is clicked and the entry should be removed.
		public event EventHandler onDelete;

		PictureBox status = new PictureBox();
		ToolTip statusLog = new ToolTip();
		TextBox extensionBox = new TextBox();
		TextBox encoderBox = new TextBox();
		Button search = new Button();
		Button delete = new Button();
		Panel parent;

		public static readonly int ItemHeight = 30;
		private static int tabIndex = 1;
	}
}
