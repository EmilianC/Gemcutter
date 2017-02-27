// Copyright (c) 2017 Emilian Cioca
using System.IO;
using System.Linq;
using System.Windows.Forms;
using System.Drawing;

namespace AssetManager
{
	class EncoderEntry
	{
		public EncoderEntry(Control parent, Point location)
		{
			CreateControls(parent, location);
			Validate();
		}

		public EncoderEntry(Control parent, Point location, string ext, string app)
		{
			CreateControls(parent, location);
			SetExtension(ext);
			SetEncoder(app);
		}

		~EncoderEntry()
		{
			tabIndex -= 4;
		}

		public void SetExtension(string ext)
		{
			extension.Text = ext;
			Validate();
		}

		public void SetEncoder(string app)
		{
			encoder.Text = app;
			Validate();
		}

		// Returns true if the contained data represents a valid extension/encoder pair.
		public bool Validate()
		{
			status.BackgroundImage = Properties.Resources.Error as Image;

			if (extension.Text.Length == 0 ||
				encoder.Text.Length == 0)
			{
				statusLog.SetToolTip(status, "Fields cannot be empty.");
				return false;
			}

			if (extension.Text.Any(x => Path.GetInvalidFileNameChars().Contains(x)))
			{
				statusLog.SetToolTip(status, "The Extension name contains invalid characters.");
				return false;
			}

			if (encoder.Text.Any(x => Path.GetInvalidPathChars().Contains(x)) ||
				!encoder.Text.EndsWith(".exe", System.StringComparison.InvariantCultureIgnoreCase))
			{
				statusLog.SetToolTip(status, "The Encoder is not a valid path to an executable.");
				return false;
			}

			var encoderPath = Path.IsPathRooted(encoder.Text) ?
				encoder.Text :
				Directory.GetCurrentDirectory() + encoder.Text;
			if (!File.Exists(encoderPath))
			{
				statusLog.SetToolTip(status, "The Encoder executable does not exist on the disk.");
				return false;
			}

			status.BackgroundImage = Properties.Resources.Correct as Image;
			statusLog.SetToolTip(status, "Extension / Encoder pair are valid.");
			return true;
		}

		private void CreateControls(Control parent, Point location)
		{
			var font = new Font("Segoe UI", 9F);

			// Manually create the GUI for the table entry.
			status.Location = new Point(3 + location.X, 8 + location.Y);
			status.BackgroundImageLayout = ImageLayout.Stretch;
			status.Size = new Size(19, 19);
			status.TabStop = false;

			extension.Location = new Point(27 + location.X, 7 + location.Y);
			extension.Size = new Size(45, 23);
			extension.TabIndex = tabIndex++;
			extension.Font = font;
			extension.TextChanged += (sender, e) => Validate();

			encoder.Location = new Point(78 + location.X, 7 + location.Y);
			encoder.Size = new Size(424, 23);
			encoder.TabIndex = tabIndex++;
			encoder.Font = font;
			encoder.TextChanged += (sender, e) => Validate();

			search.Location = new Point(508 + location.X, 7 + location.Y);
			search.Size = new Size(23, 23);
			search.BackgroundImage = Properties.Resources.Search;
			search.BackgroundImageLayout = ImageLayout.Stretch;
			search.TabIndex = tabIndex++;
			search.UseVisualStyleBackColor = true;
			search.Click += (sender, e) => SearchForEncoder();

			delete.Location = new Point(534 + location.X, 7 + location.Y);
			delete.Size = new Size(23, 23);
			delete.BackgroundImage = Properties.Resources.Delete;
			delete.BackgroundImageLayout = ImageLayout.Stretch;
			delete.TabIndex = tabIndex++;
			delete.UseVisualStyleBackColor = true;

			parent.Controls.Add(status);
			parent.Controls.Add(extension);
			parent.Controls.Add(encoder);
			parent.Controls.Add(search);
			parent.Controls.Add(delete);
		}

		private void SearchForEncoder()
		{
			OpenFileDialog dialog = new OpenFileDialog();
			dialog.Filter = "executable files (*.exe)|*.exe";
			dialog.FilterIndex = 1;
			dialog.RestoreDirectory = true;

			if (dialog.ShowDialog() == DialogResult.OK)
				encoder.Text = dialog.FileName;
		}

		PictureBox status = new PictureBox();
		ToolTip statusLog = new ToolTip();
		TextBox extension = new TextBox();
		TextBox encoder = new TextBox();
		Button search = new Button();
		Button delete = new Button();

		public static readonly int ItemHeight = 30;
		private static int tabIndex = 1;
	}
}
