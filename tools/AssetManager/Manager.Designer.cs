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
			this.ButtonUpdate = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// ButtonPack
			// 
			this.ButtonPack.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(85)))), ((int)(((byte)(85)))), ((int)(((byte)(85)))));
			this.ButtonPack.FlatAppearance.BorderSize = 0;
			this.ButtonPack.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.ButtonPack.ForeColor = System.Drawing.SystemColors.ControlLightLight;
			this.ButtonPack.Location = new System.Drawing.Point(760, 11);
			this.ButtonPack.Name = "ButtonPack";
			this.ButtonPack.Size = new System.Drawing.Size(110, 24);
			this.ButtonPack.TabIndex = 1;
			this.ButtonPack.Text = "Pack";
			this.ButtonPack.UseVisualStyleBackColor = false;
			this.ButtonPack.Click += new System.EventHandler(this.ButtonPack_Click);
			// 
			// treeViewAssets
			// 
			this.treeViewAssets.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(48)))));
			this.treeViewAssets.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.treeViewAssets.ForeColor = System.Drawing.Color.LightGray;
			this.treeViewAssets.Indent = 24;
			this.treeViewAssets.Location = new System.Drawing.Point(14, 38);
			this.treeViewAssets.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.treeViewAssets.Name = "treeViewAssets";
			this.treeViewAssets.ShowLines = false;
			this.treeViewAssets.Size = new System.Drawing.Size(354, 589);
			this.treeViewAssets.TabIndex = 7;
			// 
			// labelWorkspace
			// 
			this.labelWorkspace.AutoSize = true;
			this.labelWorkspace.ForeColor = System.Drawing.Color.LightGray;
			this.labelWorkspace.Location = new System.Drawing.Point(11, 16);
			this.labelWorkspace.Name = "labelWorkspace";
			this.labelWorkspace.Size = new System.Drawing.Size(65, 15);
			this.labelWorkspace.TabIndex = 12;
			this.labelWorkspace.Text = "Workspace";
			// 
			// buttonWorkspaceOpen
			// 
			this.buttonWorkspaceOpen.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(85)))), ((int)(((byte)(85)))), ((int)(((byte)(85)))));
			this.buttonWorkspaceOpen.FlatAppearance.BorderSize = 0;
			this.buttonWorkspaceOpen.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.buttonWorkspaceOpen.ForeColor = System.Drawing.SystemColors.ControlLightLight;
			this.buttonWorkspaceOpen.Location = new System.Drawing.Point(142, 11);
			this.buttonWorkspaceOpen.Name = "buttonWorkspaceOpen";
			this.buttonWorkspaceOpen.Size = new System.Drawing.Size(110, 24);
			this.buttonWorkspaceOpen.TabIndex = 14;
			this.buttonWorkspaceOpen.Text = "Explore";
			this.buttonWorkspaceOpen.UseVisualStyleBackColor = false;
			this.buttonWorkspaceOpen.Click += new System.EventHandler(this.buttonWorkspaceOpen_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.ForeColor = System.Drawing.Color.LightGray;
			this.label1.Location = new System.Drawing.Point(375, 16);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(45, 15);
			this.label1.TabIndex = 16;
			this.label1.Text = "Output";
			// 
			// Output
			// 
			this.Output.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(50)))));
			this.Output.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
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
			this.buttonCollapse.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(45)))), ((int)(((byte)(48)))));
			this.buttonCollapse.BackgroundImage = global::AssetManager.Properties.Resources.Collapse;
			this.buttonCollapse.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.buttonCollapse.FlatAppearance.BorderSize = 0;
			this.buttonCollapse.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(67)))), ((int)(((byte)(67)))), ((int)(((byte)(67)))));
			this.buttonCollapse.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.buttonCollapse.ForeColor = System.Drawing.SystemColors.ControlLight;
			this.buttonCollapse.Location = new System.Drawing.Point(82, 11);
			this.buttonCollapse.Name = "buttonCollapse";
			this.buttonCollapse.Size = new System.Drawing.Size(24, 24);
			this.buttonCollapse.TabIndex = 27;
			this.buttonCollapse.UseVisualStyleBackColor = false;
			this.buttonCollapse.Click += new System.EventHandler(this.buttonCollapse_Click);
			// 
			// buttonExpand
			// 
			this.buttonExpand.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(45)))), ((int)(((byte)(48)))));
			this.buttonExpand.BackgroundImage = global::AssetManager.Properties.Resources.Expand;
			this.buttonExpand.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.buttonExpand.FlatAppearance.BorderSize = 0;
			this.buttonExpand.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(67)))), ((int)(((byte)(67)))), ((int)(((byte)(67)))));
			this.buttonExpand.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.buttonExpand.ForeColor = System.Drawing.SystemColors.ControlLight;
			this.buttonExpand.Location = new System.Drawing.Point(112, 11);
			this.buttonExpand.Name = "buttonExpand";
			this.buttonExpand.Size = new System.Drawing.Size(24, 24);
			this.buttonExpand.TabIndex = 26;
			this.buttonExpand.UseVisualStyleBackColor = false;
			this.buttonExpand.Click += new System.EventHandler(this.buttonExpand_Click);
			// 
			// ButtonSettings
			// 
			this.ButtonSettings.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("ButtonSettings.BackgroundImage")));
			this.ButtonSettings.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
			this.ButtonSettings.FlatAppearance.BorderSize = 0;
			this.ButtonSettings.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(67)))), ((int)(((byte)(67)))), ((int)(((byte)(67)))));
			this.ButtonSettings.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.ButtonSettings.Location = new System.Drawing.Point(498, 11);
			this.ButtonSettings.Name = "ButtonSettings";
			this.ButtonSettings.Size = new System.Drawing.Size(24, 24);
			this.ButtonSettings.TabIndex = 23;
			this.ButtonSettings.UseVisualStyleBackColor = true;
			this.ButtonSettings.Click += new System.EventHandler(this.buttonSettings_Click);
			// 
			// buttonEditMetadata
			// 
			this.buttonEditMetadata.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(85)))), ((int)(((byte)(85)))), ((int)(((byte)(85)))));
			this.buttonEditMetadata.Enabled = false;
			this.buttonEditMetadata.FlatAppearance.BorderSize = 0;
			this.buttonEditMetadata.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.buttonEditMetadata.ForeColor = System.Drawing.SystemColors.ControlLightLight;
			this.buttonEditMetadata.Location = new System.Drawing.Point(258, 11);
			this.buttonEditMetadata.Name = "buttonEditMetadata";
			this.buttonEditMetadata.Size = new System.Drawing.Size(110, 24);
			this.buttonEditMetadata.TabIndex = 28;
			this.buttonEditMetadata.Text = "Edit Metadata";
			this.buttonEditMetadata.UseVisualStyleBackColor = false;
			// 
			// ButtonMode
			// 
			this.ButtonMode.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(85)))), ((int)(((byte)(85)))), ((int)(((byte)(85)))));
			this.ButtonMode.FlatAppearance.BorderSize = 0;
			this.ButtonMode.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.ButtonMode.ForeColor = System.Drawing.SystemColors.ControlLightLight;
			this.ButtonMode.Location = new System.Drawing.Point(528, 11);
			this.ButtonMode.Name = "ButtonMode";
			this.ButtonMode.Size = new System.Drawing.Size(110, 24);
			this.ButtonMode.TabIndex = 29;
			this.ButtonMode.Text = "Mode: Manual";
			this.ButtonMode.UseVisualStyleBackColor = false;
			this.ButtonMode.Click += new System.EventHandler(this.ButtonMode_Click);
			// 
			// ButtonUpdate
			// 
			this.ButtonUpdate.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(85)))), ((int)(((byte)(85)))), ((int)(((byte)(85)))));
			this.ButtonUpdate.FlatAppearance.BorderSize = 0;
			this.ButtonUpdate.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.ButtonUpdate.ForeColor = System.Drawing.SystemColors.ControlLightLight;
			this.ButtonUpdate.Location = new System.Drawing.Point(644, 11);
			this.ButtonUpdate.Name = "ButtonUpdate";
			this.ButtonUpdate.Size = new System.Drawing.Size(110, 24);
			this.ButtonUpdate.TabIndex = 30;
			this.ButtonUpdate.Text = "Update";
			this.ButtonUpdate.UseVisualStyleBackColor = false;
			this.ButtonUpdate.Click += new System.EventHandler(this.ButtonUpdate_Click);
			// 
			// Manager
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(45)))), ((int)(((byte)(48)))));
			this.ClientSize = new System.Drawing.Size(884, 641);
			this.Controls.Add(this.ButtonUpdate);
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
			this.Text = "Gemcutter - Asset Manager";
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
		private System.Windows.Forms.Button ButtonUpdate;
	}
}

