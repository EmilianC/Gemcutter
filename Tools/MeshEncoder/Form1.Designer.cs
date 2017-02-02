namespace Mesh_Encoder
{
	partial class MeshEncoderForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MeshEncoderForm));
			this.Button_ConvertFile = new System.Windows.Forms.Button();
			this.Button_ConvertFolder = new System.Windows.Forms.Button();
			this.Output = new System.Windows.Forms.TextBox();
			this.CheckBox_Normals = new System.Windows.Forms.CheckBox();
			this.CheckBox_UV = new System.Windows.Forms.CheckBox();
			this.Group_Options = new System.Windows.Forms.GroupBox();
			this.Group_Options.SuspendLayout();
			this.SuspendLayout();
			// 
			// Button_ConvertFile
			// 
			this.Button_ConvertFile.Location = new System.Drawing.Point(13, 12);
			this.Button_ConvertFile.Name = "Button_ConvertFile";
			this.Button_ConvertFile.Size = new System.Drawing.Size(414, 33);
			this.Button_ConvertFile.TabIndex = 0;
			this.Button_ConvertFile.Text = "Convert File";
			this.Button_ConvertFile.UseVisualStyleBackColor = true;
			this.Button_ConvertFile.Click += new System.EventHandler(this.Button_ConvertFile_Click);
			// 
			// Button_ConvertFolder
			// 
			this.Button_ConvertFolder.Location = new System.Drawing.Point(13, 51);
			this.Button_ConvertFolder.Name = "Button_ConvertFolder";
			this.Button_ConvertFolder.Size = new System.Drawing.Size(414, 33);
			this.Button_ConvertFolder.TabIndex = 1;
			this.Button_ConvertFolder.Text = "Convert Entire Folder";
			this.Button_ConvertFolder.UseVisualStyleBackColor = true;
			this.Button_ConvertFolder.Click += new System.EventHandler(this.Button_ConvertFolder_Click);
			// 
			// Output
			// 
			this.Output.Location = new System.Drawing.Point(12, 90);
			this.Output.Multiline = true;
			this.Output.Name = "Output";
			this.Output.ReadOnly = true;
			this.Output.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.Output.Size = new System.Drawing.Size(519, 223);
			this.Output.TabIndex = 2;
			// 
			// CheckBox_Normals
			// 
			this.CheckBox_Normals.AutoSize = true;
			this.CheckBox_Normals.Checked = true;
			this.CheckBox_Normals.CheckState = System.Windows.Forms.CheckState.Checked;
			this.CheckBox_Normals.Location = new System.Drawing.Point(6, 42);
			this.CheckBox_Normals.Name = "CheckBox_Normals";
			this.CheckBox_Normals.Size = new System.Drawing.Size(64, 17);
			this.CheckBox_Normals.TabIndex = 3;
			this.CheckBox_Normals.Text = "Normals";
			this.CheckBox_Normals.UseVisualStyleBackColor = true;
			// 
			// CheckBox_UV
			// 
			this.CheckBox_UV.AutoSize = true;
			this.CheckBox_UV.Checked = true;
			this.CheckBox_UV.CheckState = System.Windows.Forms.CheckState.Checked;
			this.CheckBox_UV.Location = new System.Drawing.Point(6, 19);
			this.CheckBox_UV.Name = "CheckBox_UV";
			this.CheckBox_UV.Size = new System.Drawing.Size(48, 17);
			this.CheckBox_UV.TabIndex = 5;
			this.CheckBox_UV.Text = "UV\'s";
			this.CheckBox_UV.UseVisualStyleBackColor = true;
			// 
			// Group_Options
			// 
			this.Group_Options.Controls.Add(this.CheckBox_UV);
			this.Group_Options.Controls.Add(this.CheckBox_Normals);
			this.Group_Options.Location = new System.Drawing.Point(433, 12);
			this.Group_Options.Name = "Group_Options";
			this.Group_Options.Size = new System.Drawing.Size(99, 72);
			this.Group_Options.TabIndex = 6;
			this.Group_Options.TabStop = false;
			this.Group_Options.Text = "Output";
			// 
			// MeshEncoderForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(544, 325);
			this.Controls.Add(this.Group_Options);
			this.Controls.Add(this.Output);
			this.Controls.Add(this.Button_ConvertFolder);
			this.Controls.Add(this.Button_ConvertFile);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.Name = "MeshEncoderForm";
			this.Text = "Jewel3D - Mesh Encoder";
			this.Group_Options.ResumeLayout(false);
			this.Group_Options.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button Button_ConvertFile;
		private System.Windows.Forms.Button Button_ConvertFolder;
		private System.Windows.Forms.TextBox Output;
		private System.Windows.Forms.CheckBox CheckBox_Normals;
		private System.Windows.Forms.CheckBox CheckBox_UV;
		private System.Windows.Forms.GroupBox Group_Options;
	}
}

