namespace SourceForgePlugin
{
	partial class SourceForgeConfigDialog
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SourceForgeConfigDialog));
			this.label_name = new System.Windows.Forms.Label();
			this.label_info = new System.Windows.Forms.Label();
			this.label_ID = new System.Windows.Forms.Label();
			this.textBox_name = new System.Windows.Forms.TextBox();
			this.textBox_ID = new System.Windows.Forms.TextBox();
			this.groupBox_project = new System.Windows.Forms.GroupBox();
			this.label_error = new System.Windows.Forms.Label();
			this.checkBox_summary = new System.Windows.Forms.CheckBox();
			this.checkBox_lastComment = new System.Windows.Forms.CheckBox();
			this.button_OK = new System.Windows.Forms.Button();
			this.checkBox_details = new System.Windows.Forms.CheckBox();
			this.label_version = new System.Windows.Forms.Label();
			this.groupBox_project.SuspendLayout();
			this.SuspendLayout();
			// 
			// label_name
			// 
			this.label_name.AutoSize = true;
			this.label_name.Location = new System.Drawing.Point(9, 74);
			this.label_name.Name = "label_name";
			this.label_name.Size = new System.Drawing.Size(74, 13);
			this.label_name.TabIndex = 0;
			this.label_name.Text = "Project Name";
			// 
			// label_info
			// 
			this.label_info.Location = new System.Drawing.Point(9, 18);
			this.label_info.Name = "label_info";
			this.label_info.Size = new System.Drawing.Size(415, 41);
			this.label_info.TabIndex = 2;
			this.label_info.Text = resources.GetString("label_info.Text");
			// 
			// label_ID
			// 
			this.label_ID.AutoSize = true;
			this.label_ID.Location = new System.Drawing.Point(9, 107);
			this.label_ID.Name = "label_ID";
			this.label_ID.Size = new System.Drawing.Size(56, 13);
			this.label_ID.TabIndex = 3;
			this.label_ID.Text = "Project ID";
			// 
			// textBox_name
			// 
			this.textBox_name.Location = new System.Drawing.Point(90, 70);
			this.textBox_name.Name = "textBox_name";
			this.textBox_name.Size = new System.Drawing.Size(179, 22);
			this.textBox_name.TabIndex = 4;
			// 
			// textBox_ID
			// 
			this.textBox_ID.CausesValidation = false;
			this.textBox_ID.Enabled = false;
			this.textBox_ID.Location = new System.Drawing.Point(90, 103);
			this.textBox_ID.Name = "textBox_ID";
			this.textBox_ID.Size = new System.Drawing.Size(179, 22);
			this.textBox_ID.TabIndex = 5;
			// 
			// groupBox_project
			// 
			this.groupBox_project.Controls.Add(this.label_error);
			this.groupBox_project.Controls.Add(this.label_info);
			this.groupBox_project.Controls.Add(this.textBox_ID);
			this.groupBox_project.Controls.Add(this.label_name);
			this.groupBox_project.Controls.Add(this.textBox_name);
			this.groupBox_project.Controls.Add(this.label_ID);
			this.groupBox_project.Location = new System.Drawing.Point(12, 12);
			this.groupBox_project.Name = "groupBox_project";
			this.groupBox_project.Size = new System.Drawing.Size(430, 156);
			this.groupBox_project.TabIndex = 6;
			this.groupBox_project.TabStop = false;
			this.groupBox_project.Text = "Project Configuration";
			// 
			// label_error
			// 
			this.label_error.AutoSize = true;
			this.label_error.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label_error.ForeColor = System.Drawing.Color.Red;
			this.label_error.Location = new System.Drawing.Point(9, 133);
			this.label_error.Name = "label_error";
			this.label_error.Size = new System.Drawing.Size(377, 13);
			this.label_error.TabIndex = 6;
			this.label_error.Text = "Cannot validate the project! Make sure you entered the name correctly.";
			this.label_error.Visible = false;
			// 
			// checkBox_summary
			// 
			this.checkBox_summary.AutoSize = true;
			this.checkBox_summary.Location = new System.Drawing.Point(12, 200);
			this.checkBox_summary.Name = "checkBox_summary";
			this.checkBox_summary.Size = new System.Drawing.Size(132, 17);
			this.checkBox_summary.TabIndex = 7;
			this.checkBox_summary.Text = "Show entry summary";
			this.checkBox_summary.UseVisualStyleBackColor = true;
			// 
			// checkBox_lastComment
			// 
			this.checkBox_lastComment.AutoSize = true;
			this.checkBox_lastComment.Location = new System.Drawing.Point(12, 223);
			this.checkBox_lastComment.Name = "checkBox_lastComment";
			this.checkBox_lastComment.Size = new System.Drawing.Size(126, 17);
			this.checkBox_lastComment.TabIndex = 8;
			this.checkBox_lastComment.Text = "Show last comment";
			this.checkBox_lastComment.UseVisualStyleBackColor = true;
			// 
			// button_OK
			// 
			this.button_OK.Location = new System.Drawing.Point(367, 232);
			this.button_OK.Name = "button_OK";
			this.button_OK.Size = new System.Drawing.Size(75, 23);
			this.button_OK.TabIndex = 9;
			this.button_OK.Text = "OK";
			this.button_OK.UseVisualStyleBackColor = true;
			this.button_OK.Click += new System.EventHandler(this.OK_Click);
			// 
			// checkBox_details
			// 
			this.checkBox_details.AutoSize = true;
			this.checkBox_details.Location = new System.Drawing.Point(12, 177);
			this.checkBox_details.Name = "checkBox_details";
			this.checkBox_details.Size = new System.Drawing.Size(280, 17);
			this.checkBox_details.TabIndex = 10;
			this.checkBox_details.Text = "Show entry details (resolution, category, group...)";
			this.checkBox_details.UseVisualStyleBackColor = true;
			// 
			// label_version
			// 
			this.label_version.AutoSize = true;
			this.label_version.Enabled = false;
			this.label_version.Location = new System.Drawing.Point(12, 247);
			this.label_version.Name = "label_version";
			this.label_version.Size = new System.Drawing.Size(100, 13);
			this.label_version.TabIndex = 11;
			this.label_version.Text = "Version x.y Build z";
			// 
			// SourceForgeConfigDialog
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(454, 267);
			this.Controls.Add(this.label_version);
			this.Controls.Add(this.checkBox_details);
			this.Controls.Add(this.button_OK);
			this.Controls.Add(this.checkBox_lastComment);
			this.Controls.Add(this.checkBox_summary);
			this.Controls.Add(this.groupBox_project);
			this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "SourceForgeConfigDialog";
			this.ShowIcon = false;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "SourceForge Plugin Configuration";
			this.TopMost = true;
			this.groupBox_project.ResumeLayout(false);
			this.groupBox_project.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label label_name;
		private System.Windows.Forms.Label label_info;
		private System.Windows.Forms.Label label_ID;
		private System.Windows.Forms.TextBox textBox_name;
		private System.Windows.Forms.TextBox textBox_ID;
		private System.Windows.Forms.GroupBox groupBox_project;
		private System.Windows.Forms.CheckBox checkBox_summary;
		private System.Windows.Forms.CheckBox checkBox_lastComment;
		private System.Windows.Forms.Label label_error;
		private System.Windows.Forms.Button button_OK;
		private System.Windows.Forms.CheckBox checkBox_details;
		private System.Windows.Forms.Label label_version;
	}
}