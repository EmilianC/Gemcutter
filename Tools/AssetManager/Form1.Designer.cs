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
			this.listBoxAssets = new System.Windows.Forms.ListBox();
			this.labelWorkspace = new System.Windows.Forms.Label();
			this.labelAssets = new System.Windows.Forms.Label();
			this.buttonWorkspaceOpen = new System.Windows.Forms.Button();
			this.buttonAssetOpen = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.buttonClear = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// ButtonPack
			// 
			this.ButtonPack.Location = new System.Drawing.Point(662, 11);
			this.ButtonPack.Name = "ButtonPack";
			this.ButtonPack.Size = new System.Drawing.Size(110, 21);
			this.ButtonPack.TabIndex = 1;
			this.ButtonPack.Text = "Pack";
			this.ButtonPack.UseVisualStyleBackColor = true;
			this.ButtonPack.Click += new System.EventHandler(this.Button_Pack_Click);
			// 
			// Output
			// 
			this.Output.Location = new System.Drawing.Point(392, 34);
			this.Output.Multiline = true;
			this.Output.Name = "Output";
			this.Output.ReadOnly = true;
			this.Output.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.Output.Size = new System.Drawing.Size(380, 511);
			this.Output.TabIndex = 2;
			// 
			// treeViewAssets
			// 
			this.treeViewAssets.Location = new System.Drawing.Point(12, 34);
			this.treeViewAssets.Name = "treeViewAssets";
			this.treeViewAssets.Size = new System.Drawing.Size(184, 511);
			this.treeViewAssets.TabIndex = 7;
			// 
			// listBoxAssets
			// 
			this.listBoxAssets.FormattingEnabled = true;
			this.listBoxAssets.Location = new System.Drawing.Point(202, 34);
			this.listBoxAssets.Name = "listBoxAssets";
			this.listBoxAssets.Size = new System.Drawing.Size(184, 329);
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
			// buttonWorkspaceOpen
			// 
			this.buttonWorkspaceOpen.Location = new System.Drawing.Point(86, 11);
			this.buttonWorkspaceOpen.Name = "buttonWorkspaceOpen";
			this.buttonWorkspaceOpen.Size = new System.Drawing.Size(110, 21);
			this.buttonWorkspaceOpen.TabIndex = 14;
			this.buttonWorkspaceOpen.Text = "Open";
			this.buttonWorkspaceOpen.UseVisualStyleBackColor = true;
			this.buttonWorkspaceOpen.Click += new System.EventHandler(this.buttonWorkspaceOpen_Click);
			// 
			// buttonAssetOpen
			// 
			this.buttonAssetOpen.Location = new System.Drawing.Point(276, 11);
			this.buttonAssetOpen.Name = "buttonAssetOpen";
			this.buttonAssetOpen.Size = new System.Drawing.Size(110, 21);
			this.buttonAssetOpen.TabIndex = 15;
			this.buttonAssetOpen.Text = "Open";
			this.buttonAssetOpen.UseVisualStyleBackColor = true;
			this.buttonAssetOpen.Click += new System.EventHandler(this.buttonAssetOpen_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(392, 18);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(39, 13);
			this.label1.TabIndex = 16;
			this.label1.Text = "Output";
			// 
			// groupBox1
			// 
			this.groupBox1.Location = new System.Drawing.Point(202, 369);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(184, 176);
			this.groupBox1.TabIndex = 17;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Properties";
			// 
			// buttonClear
			// 
			this.buttonClear.Location = new System.Drawing.Point(546, 11);
			this.buttonClear.Name = "buttonClear";
			this.buttonClear.Size = new System.Drawing.Size(110, 21);
			this.buttonClear.TabIndex = 18;
			this.buttonClear.Text = "Clear";
			this.buttonClear.UseVisualStyleBackColor = true;
			this.buttonClear.Click += new System.EventHandler(this.buttonClear_Click);
			// 
			// formAssetManager
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(784, 557);
			this.Controls.Add(this.buttonClear);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.Output);
			this.Controls.Add(this.ButtonPack);
			this.Controls.Add(this.buttonAssetOpen);
			this.Controls.Add(this.buttonWorkspaceOpen);
			this.Controls.Add(this.labelAssets);
			this.Controls.Add(this.labelWorkspace);
			this.Controls.Add(this.listBoxAssets);
			this.Controls.Add(this.treeViewAssets);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.Name = "formAssetManager";
			this.Text = "Jewel3D - Asset Manager";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion
		private System.Windows.Forms.Button ButtonPack;
		private System.Windows.Forms.TextBox Output;
		private System.Windows.Forms.TreeView treeViewAssets;
		private System.Windows.Forms.ListBox listBoxAssets;
		private System.Windows.Forms.Label labelWorkspace;
		private System.Windows.Forms.Label labelAssets;
		private System.Windows.Forms.Button buttonWorkspaceOpen;
		private System.Windows.Forms.Button buttonAssetOpen;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button buttonClear;
	}
}

