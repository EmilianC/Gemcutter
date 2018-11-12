namespace AssetManager
{
	partial class Manager
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Manager));
			this.ButtonPack = new System.Windows.Forms.Button();
			this.treeViewAssets = new System.Windows.Forms.TreeView();
			this.labelWorkspace = new System.Windows.Forms.Label();
			this.buttonWorkspaceOpen = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.Output = new System.Windows.Forms.RichTextBox();
			this.buttonCollapse = new System.Windows.Forms.Button();
			this.buttonExpand = new System.Windows.Forms.Button();
			this.ButtonSettings = new System.Windows.Forms.Button();
			this.buttonEditMetadata = new System.Windows.Forms.Button();
			this.ButtonMode = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// ButtonPack
			// 
			this.ButtonPack.Location = new System.Drawing.Point(760, 12);
			this.ButtonPack.Name = "ButtonPack";
			this.ButtonPack.Size = new System.Drawing.Size(110, 24);
			this.ButtonPack.TabIndex = 1;
			this.ButtonPack.Text = "Pack";
			this.ButtonPack.UseVisualStyleBackColor = true;
			this.ButtonPack.Click += new System.EventHandler(this.ButtonPack_Click);
			// 
			// treeViewAssets
			// 
			this.treeViewAssets.BackColor = System.Drawing.SystemColors.Window;
			this.treeViewAssets.Location = new System.Drawing.Point(14, 38);
			this.treeViewAssets.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.treeViewAssets.Name = "treeViewAssets";
			this.treeViewAssets.Size = new System.Drawing.Size(354, 589);
			this.treeViewAssets.TabIndex = 7;
			// 
			// labelWorkspace
			// 
			this.labelWorkspace.AutoSize = true;
			this.labelWorkspace.Location = new System.Drawing.Point(11, 17);
			this.labelWorkspace.Name = "labelWorkspace";
			this.labelWorkspace.Size = new System.Drawing.Size(65, 15);
			this.labelWorkspace.TabIndex = 12;
			this.labelWorkspace.Text = "Workspace";
			// 
			// buttonWorkspaceOpen
			// 
			this.buttonWorkspaceOpen.Location = new System.Drawing.Point(142, 12);
			this.buttonWorkspaceOpen.Name = "buttonWorkspaceOpen";
			this.buttonWorkspaceOpen.Size = new System.Drawing.Size(110, 24);
			this.buttonWorkspaceOpen.TabIndex = 14;
			this.buttonWorkspaceOpen.Text = "Explore";
			this.buttonWorkspaceOpen.UseVisualStyleBackColor = true;
			this.buttonWorkspaceOpen.Click += new System.EventHandler(this.buttonWorkspaceOpen_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(375, 17);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(45, 15);
			this.label1.TabIndex = 16;
			this.label1.Text = "Output";
			// 
			// Output
			// 
			this.Output.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(48)))));
			this.Output.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.Output.ForeColor = System.Drawing.Color.LightGray;
			this.Output.Location = new System.Drawing.Point(378, 38);
			this.Output.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.Output.Name = "Output";
			this.Output.ReadOnly = true;
			this.Output.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.Vertical;
			this.Output.Size = new System.Drawing.Size(492, 589);
			this.Output.TabIndex = 19;
			this.Output.Text = "";
			// 
			// buttonCollapse
			// 
			this.buttonCollapse.BackgroundImage = global::AssetManager.Properties.Resources.Collapse;
			this.buttonCollapse.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.buttonCollapse.Location = new System.Drawing.Point(82, 12);
			this.buttonCollapse.Name = "buttonCollapse";
			this.buttonCollapse.Size = new System.Drawing.Size(24, 24);
			this.buttonCollapse.TabIndex = 27;
			this.buttonCollapse.UseVisualStyleBackColor = true;
			this.buttonCollapse.Click += new System.EventHandler(this.buttonCollapse_Click);
			// 
			// buttonExpand
			// 
			this.buttonExpand.BackgroundImage = global::AssetManager.Properties.Resources.Expand;
			this.buttonExpand.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.buttonExpand.Location = new System.Drawing.Point(112, 12);
			this.buttonExpand.Name = "buttonExpand";
			this.buttonExpand.Size = new System.Drawing.Size(24, 24);
			this.buttonExpand.TabIndex = 26;
			this.buttonExpand.UseVisualStyleBackColor = true;
			this.buttonExpand.Click += new System.EventHandler(this.buttonExpand_Click);
			// 
			// ButtonSettings
			// 
			this.ButtonSettings.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("ButtonSettings.BackgroundImage")));
			this.ButtonSettings.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
			this.ButtonSettings.Location = new System.Drawing.Point(614, 12);
			this.ButtonSettings.Name = "ButtonSettings";
			this.ButtonSettings.Size = new System.Drawing.Size(24, 24);
			this.ButtonSettings.TabIndex = 23;
			this.ButtonSettings.UseVisualStyleBackColor = true;
			this.ButtonSettings.Click += new System.EventHandler(this.buttonSettings_Click);
			// 
			// buttonEditMetadata
			// 
			this.buttonEditMetadata.Enabled = false;
			this.buttonEditMetadata.Location = new System.Drawing.Point(258, 12);
			this.buttonEditMetadata.Name = "buttonEditMetadata";
			this.buttonEditMetadata.Size = new System.Drawing.Size(110, 24);
			this.buttonEditMetadata.TabIndex = 28;
			this.buttonEditMetadata.Text = "Edit Metadata";
			this.buttonEditMetadata.UseVisualStyleBackColor = true;
			// 
			// ButtonMode
			// 
			this.ButtonMode.Location = new System.Drawing.Point(644, 12);
			this.ButtonMode.Name = "ButtonMode";
			this.ButtonMode.Size = new System.Drawing.Size(110, 24);
			this.ButtonMode.TabIndex = 29;
			this.ButtonMode.Text = "Mode: Manual";
			this.ButtonMode.UseVisualStyleBackColor = true;
			this.ButtonMode.Click += new System.EventHandler(this.ButtonMode_Click);
			// 
			// Manager
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(884, 641);
			this.Controls.Add(this.ButtonMode);
			this.Controls.Add(this.buttonEditMetadata);
			this.Controls.Add(this.buttonCollapse);
			this.Controls.Add(this.buttonExpand);
			this.Controls.Add(this.treeViewAssets);
			this.Controls.Add(this.ButtonSettings);
			this.Controls.Add(this.Output);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.ButtonPack);
			this.Controls.Add(this.buttonWorkspaceOpen);
			this.Controls.Add(this.labelWorkspace);
			this.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.Name = "Manager";
			this.Text = "Jewel3D - Asset Manager";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion
		private System.Windows.Forms.Button ButtonPack;
		private System.Windows.Forms.TreeView treeViewAssets;
		private System.Windows.Forms.Label labelWorkspace;
		private System.Windows.Forms.Button buttonWorkspaceOpen;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.RichTextBox Output;
		private System.Windows.Forms.Button ButtonSettings;
		private System.Windows.Forms.Button buttonExpand;
		private System.Windows.Forms.Button buttonCollapse;
		private System.Windows.Forms.Button buttonEditMetadata;
		private System.Windows.Forms.Button ButtonMode;
	}
}

