namespace AssetManager
{
	partial class Settings
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Settings));
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.panel = new System.Windows.Forms.Panel();
			this.buttonAdd = new System.Windows.Forms.Button();
			this.LabelOutputDirectory = new System.Windows.Forms.Label();
			this.LabelExcludedExtensions = new System.Windows.Forms.Label();
			this.TextBoxOutputDirectory = new System.Windows.Forms.TextBox();
			this.TextBoxExcludedExtensions = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Segoe UI", 9F);
			this.label1.ForeColor = System.Drawing.Color.LightGray;
			this.label1.Location = new System.Drawing.Point(38, 69);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(35, 15);
			this.label1.TabIndex = 3;
			this.label1.Text = "Asset";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("Segoe UI", 9F);
			this.label2.ForeColor = System.Drawing.Color.LightGray;
			this.label2.Location = new System.Drawing.Point(88, 69);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(50, 15);
			this.label2.TabIndex = 4;
			this.label2.Text = "Encoder";
			// 
			// panel
			// 
			this.panel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(50)))));
			this.panel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.panel.Location = new System.Drawing.Point(12, 90);
			this.panel.Name = "panel";
			this.panel.Size = new System.Drawing.Size(660, 217);
			this.panel.TabIndex = 5;
			// 
			// buttonAdd
			// 
			this.buttonAdd.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(85)))), ((int)(((byte)(85)))), ((int)(((byte)(85)))));
			this.buttonAdd.FlatAppearance.BorderSize = 0;
			this.buttonAdd.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.buttonAdd.Font = new System.Drawing.Font("Segoe UI", 9F);
			this.buttonAdd.ForeColor = System.Drawing.SystemColors.ControlLightLight;
			this.buttonAdd.Location = new System.Drawing.Point(561, 64);
			this.buttonAdd.Name = "buttonAdd";
			this.buttonAdd.Size = new System.Drawing.Size(111, 24);
			this.buttonAdd.TabIndex = 9999;
			this.buttonAdd.Text = "Add";
			this.buttonAdd.UseVisualStyleBackColor = false;
			this.buttonAdd.Click += new System.EventHandler(this.buttonAdd_Click);
			// 
			// LabelOutputDirectory
			// 
			this.LabelOutputDirectory.AutoSize = true;
			this.LabelOutputDirectory.ForeColor = System.Drawing.Color.LightGray;
			this.LabelOutputDirectory.Location = new System.Drawing.Point(12, 9);
			this.LabelOutputDirectory.Name = "LabelOutputDirectory";
			this.LabelOutputDirectory.Size = new System.Drawing.Size(95, 15);
			this.LabelOutputDirectory.TabIndex = 10000;
			this.LabelOutputDirectory.Text = "Ouput Directory:";
			// 
			// LabelExcludedExtensions
			// 
			this.LabelExcludedExtensions.AutoSize = true;
			this.LabelExcludedExtensions.ForeColor = System.Drawing.Color.LightGray;
			this.LabelExcludedExtensions.Location = new System.Drawing.Point(12, 38);
			this.LabelExcludedExtensions.Name = "LabelExcludedExtensions";
			this.LabelExcludedExtensions.Size = new System.Drawing.Size(115, 15);
			this.LabelExcludedExtensions.TabIndex = 10001;
			this.LabelExcludedExtensions.Text = "Excluded Extensions:";
			// 
			// TextBoxOutputDirectory
			// 
			this.TextBoxOutputDirectory.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(37)))), ((int)(((byte)(37)))), ((int)(((byte)(38)))));
			this.TextBoxOutputDirectory.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.TextBoxOutputDirectory.ForeColor = System.Drawing.SystemColors.Info;
			this.TextBoxOutputDirectory.Location = new System.Drawing.Point(133, 6);
			this.TextBoxOutputDirectory.Name = "TextBoxOutputDirectory";
			this.TextBoxOutputDirectory.Size = new System.Drawing.Size(539, 23);
			this.TextBoxOutputDirectory.TabIndex = 10002;
			this.TextBoxOutputDirectory.TextChanged += new System.EventHandler(this.TextBoxOutputDirectory_TextChanged);
			// 
			// TextBoxExcludedExtensions
			// 
			this.TextBoxExcludedExtensions.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(37)))), ((int)(((byte)(37)))), ((int)(((byte)(38)))));
			this.TextBoxExcludedExtensions.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.TextBoxExcludedExtensions.ForeColor = System.Drawing.SystemColors.Info;
			this.TextBoxExcludedExtensions.Location = new System.Drawing.Point(133, 35);
			this.TextBoxExcludedExtensions.Name = "TextBoxExcludedExtensions";
			this.TextBoxExcludedExtensions.Size = new System.Drawing.Size(539, 23);
			this.TextBoxExcludedExtensions.TabIndex = 10003;
			this.TextBoxExcludedExtensions.TextChanged += new System.EventHandler(this.TextBoxExcludedExtensions_TextChanged);
			// 
			// Settings
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(45)))), ((int)(((byte)(48)))));
			this.ClientSize = new System.Drawing.Size(684, 311);
			this.Controls.Add(this.TextBoxExcludedExtensions);
			this.Controls.Add(this.TextBoxOutputDirectory);
			this.Controls.Add(this.LabelExcludedExtensions);
			this.Controls.Add(this.LabelOutputDirectory);
			this.Controls.Add(this.buttonAdd);
			this.Controls.Add(this.panel);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Font = new System.Drawing.Font("Segoe UI", 9F);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "Settings";
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.Text = "Settings";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Panel panel;
		private System.Windows.Forms.Button buttonAdd;
		private System.Windows.Forms.Label LabelOutputDirectory;
		private System.Windows.Forms.Label LabelExcludedExtensions;
		private System.Windows.Forms.TextBox TextBoxOutputDirectory;
		private System.Windows.Forms.TextBox TextBoxExcludedExtensions;
	}
}