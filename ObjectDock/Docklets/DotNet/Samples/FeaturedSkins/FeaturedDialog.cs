///////////////////////////////////////////////////////////////////////////////////////////////
//
// Featured Skins .Net Docklet for ObjectDock
//
// Copyright (c) 2004-2008, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer. 
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution. 
//  3. The name of the author may not be used to endorse or promote products derived from this
//     software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////

using System;
using System.IO;
using System.Drawing;
using System.Drawing.Imaging;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace FeaturedSkins
{
	/// <summary>
	/// Summary description for FeaturedDialog.
	/// </summary>
	public class FeaturedDialog : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button cancelBtn;
		private System.Windows.Forms.Button showBtn;
		private System.Windows.Forms.PictureBox skinPicture;
		Uri skinURL;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public FeaturedDialog(Uri skinURL, string skinImage)
		{

			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//

			if (File.Exists(skinImage)) 	
				skinPicture.Image = Image.FromFile(skinImage);

			this.skinURL = skinURL;
			if (skinURL != null)
				showBtn.Enabled = true;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(FeaturedDialog));
			this.showBtn = new System.Windows.Forms.Button();
			this.cancelBtn = new System.Windows.Forms.Button();
			this.skinPicture = new System.Windows.Forms.PictureBox();
			this.SuspendLayout();
			// 
			// showBtn
			// 
			this.showBtn.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.showBtn.Enabled = false;
			this.showBtn.Location = new System.Drawing.Point(120, 304);
			this.showBtn.Name = "showBtn";
			this.showBtn.Size = new System.Drawing.Size(144, 23);
			this.showBtn.TabIndex = 0;
			this.showBtn.Text = "Show webpage";
			this.showBtn.Click += new System.EventHandler(this.downloadBtn_Click);
			// 
			// cancelBtn
			// 
			this.cancelBtn.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelBtn.Location = new System.Drawing.Point(272, 304);
			this.cancelBtn.Name = "cancelBtn";
			this.cancelBtn.TabIndex = 1;
			this.cancelBtn.Text = "Close";
			this.cancelBtn.Click += new System.EventHandler(this.cancelBtn_Click);
			// 
			// skinPicture
			// 
			this.skinPicture.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.skinPicture.Location = new System.Drawing.Point(8, 8);
			this.skinPicture.Name = "skinPicture";
			this.skinPicture.Size = new System.Drawing.Size(336, 288);
			this.skinPicture.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this.skinPicture.TabIndex = 3;
			this.skinPicture.TabStop = false;
			// 
			// FeaturedDialog
			// 
			this.AcceptButton = this.showBtn;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.cancelBtn;
			this.ClientSize = new System.Drawing.Size(352, 331);
			this.Controls.Add(this.skinPicture);
			this.Controls.Add(this.showBtn);
			this.Controls.Add(this.cancelBtn);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "FeaturedDialog";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Featured Skin";
			this.ResumeLayout(false);

		}
		#endregion

		private void downloadBtn_Click(object sender, System.EventArgs e)
		{
			System.Diagnostics.Process.Start( "IEXPLORE.EXE", skinURL.ToString());
		}

		private void cancelBtn_Click(object sender, System.EventArgs e)
		{
			this.Close();		
		}

	}
}
