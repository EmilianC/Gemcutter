namespace AssetManager
{
	partial class formAssetManager
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formAssetManager));
			this.ButtonPack = new System.Windows.Forms.Button();
			this.Output = new System.Windows.Forms.TextBox();
			this.treeViewAssets = new System.Windows.Forms.TreeView();
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.tabProperties = new System.Windows.Forms.TabPage();
			this.tabOutput = new System.Windows.Forms.TabPage();
			this.listBoxAssets = new System.Windows.Forms.ListBox();
			this.labelWorkspace = new System.Windows.Forms.Label();
			this.labelAssets = new System.Windows.Forms.Label();
			this.tabControl1.SuspendLayout();
			this.tabOutput.SuspendLayout();
			this.SuspendLayout();
			// 
			// ButtonPack
			// 
			this.ButtonPack.Location = new System.Drawing.Point(392, 518);
			this.ButtonPack.Name = "ButtonPack";
			this.ButtonPack.Size = new System.Drawing.Size(383, 27);
			this.ButtonPack.TabIndex = 1;
			this.ButtonPack.Text = "Pack";
			this.ButtonPack.UseVisualStyleBackColor = true;
			this.ButtonPack.Click += new System.EventHandler(this.Button_ConvertFolder_Click);
			// 
			// Output
			// 
			this.Output.Location = new System.Drawing.Point(6, 6);
			this.Output.Multiline = true;
			this.Output.Name = "Output";
			this.Output.ReadOnly = true;
			this.Output.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.Output.Size = new System.Drawing.Size(367, 465);
			this.Output.TabIndex = 2;
			// 
			// treeViewAssets
			// 
			this.treeViewAssets.Location = new System.Drawing.Point(12, 34);
			this.treeViewAssets.Name = "treeViewAssets";
			this.treeViewAssets.Size = new System.Drawing.Size(184, 511);
			this.treeViewAssets.TabIndex = 7;
			// 
			// tabControl1
			// 
			this.tabControl1.Controls.Add(this.tabProperties);
			this.tabControl1.Controls.Add(this.tabOutput);
			this.tabControl1.Location = new System.Drawing.Point(392, 12);
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(387, 500);
			this.tabControl1.TabIndex = 8;
			// 
			// tabProperties
			// 
			this.tabProperties.Location = new System.Drawing.Point(4, 22);
			this.tabProperties.Name = "tabProperties";
			this.tabProperties.Padding = new System.Windows.Forms.Padding(3);
			this.tabProperties.Size = new System.Drawing.Size(379, 474);
			this.tabProperties.TabIndex = 0;
			this.tabProperties.Text = "Properties";
			this.tabProperties.UseVisualStyleBackColor = true;
			// 
			// tabOutput
			// 
			this.tabOutput.Controls.Add(this.Output);
			this.tabOutput.Location = new System.Drawing.Point(4, 22);
			this.tabOutput.Name = "tabOutput";
			this.tabOutput.Padding = new System.Windows.Forms.Padding(3);
			this.tabOutput.Size = new System.Drawing.Size(379, 474);
			this.tabOutput.TabIndex = 1;
			this.tabOutput.Text = "Ouput";
			this.tabOutput.UseVisualStyleBackColor = true;
			// 
			// listBoxAssets
			// 
			this.listBoxAssets.FormattingEnabled = true;
			this.listBoxAssets.Location = new System.Drawing.Point(202, 34);
			this.listBoxAssets.Name = "listBoxAssets";
			this.listBoxAssets.Size = new System.Drawing.Size(184, 511);
			this.listBoxAssets.TabIndex = 11;
			// 
			// labelWorkspace
			// 
			this.labelWorkspace.AutoSize = true;
			this.labelWorkspace.Location = new System.Drawing.Point(9, 18);
			this.labelWorkspace.Name = "labelWorkspace";
			this.labelWorkspace.Size = new System.Drawing.Size(62, 13);
			this.labelWorkspace.TabIndex = 12;
			this.labelWorkspace.Text = "Workspace";
			// 
			// labelAssets
			// 
			this.labelAssets.AutoSize = true;
			this.labelAssets.Location = new System.Drawing.Point(199, 18);
			this.labelAssets.Name = "labelAssets";
			this.labelAssets.Size = new System.Drawing.Size(38, 13);
			this.labelAssets.TabIndex = 13;
			this.labelAssets.Text = "Assets";
			// 
			// formAssetManager
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(784, 557);
			this.Controls.Add(this.labelAssets);
			this.Controls.Add(this.labelWorkspace);
			this.Controls.Add(this.listBoxAssets);
			this.Controls.Add(this.tabControl1);
			this.Controls.Add(this.treeViewAssets);
			this.Controls.Add(this.ButtonPack);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.Name = "formAssetManager";
			this.Text = "Jewel3D - Asset Manager";
			this.tabControl1.ResumeLayout(false);
			this.tabOutput.ResumeLayout(false);
			this.tabOutput.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion
		private System.Windows.Forms.Button ButtonPack;
		private System.Windows.Forms.TextBox Output;
		private System.Windows.Forms.TreeView treeViewAssets;
		private System.Windows.Forms.TabControl tabControl1;
		private System.Windows.Forms.TabPage tabProperties;
		private System.Windows.Forms.TabPage tabOutput;
		private System.Windows.Forms.ListBox listBoxAssets;
		private System.Windows.Forms.Label labelWorkspace;
		private System.Windows.Forms.Label labelAssets;
	}
}

