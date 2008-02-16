///////////////////////////////////////////////////////////////////////////////////////////////
//
// Birthday Reminder Docklet for ObjectDock
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
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using BirthdayReminder;

namespace BirthdayReminder
{
	/// <summary>
	/// Summary description for BirthdaysDialog.
	/// </summary>
	public class BirthdaysDialog : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Button closeBtn;
		private System.Windows.Forms.Panel panel;
		private BirthdayControl birthdayControl;
		private System.Windows.Forms.CheckBox animateCheck;
		BirthdayReminder.BirthdayData data;

		public BirthdaysDialog(BirthdayReminder.BirthdayData data, BirthdayReminder.AnimateChanged aniDelegate, Boolean animate)
		{
			this.data = data;
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			animateCheck.Checked = animate;
			animateCheck.CheckedChanged += new EventHandler(aniDelegate);
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(BirthdaysDialog));
			this.closeBtn = new System.Windows.Forms.Button();
			this.animateCheck = new System.Windows.Forms.CheckBox();
			this.panel = new System.Windows.Forms.Panel();
			this.birthdayControl = new BirthdayControl();
			this.SuspendLayout();
			// 
			// closeBtn
			// 
			this.closeBtn.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.closeBtn.Location = new System.Drawing.Point(304, 304);
			this.closeBtn.Name = "closeBtn";
			this.closeBtn.TabIndex = 0;
			this.closeBtn.Text = "Close";
			this.closeBtn.Click += new System.EventHandler(this.closeBtn_Click);
			// 
			// animateCheck
			// 
			this.animateCheck.Location = new System.Drawing.Point(8, 304);
			this.animateCheck.Name = "animateCheck";
			this.animateCheck.Size = new System.Drawing.Size(160, 24);
			this.animateCheck.TabIndex = 1;
			this.animateCheck.Text = "Animate Docklet";
			// 
			// birthdayControl
			// 
			this.birthdayControl.Location = new System.Drawing.Point(0, 0);
			this.birthdayControl.Name = "birthdayControl";
			this.birthdayControl.TabIndex = 0;
			this.birthdayControl.Data = this.data;
			// 
			// panel
			// 
			this.panel.AutoScroll = true;
			this.panel.Location = new System.Drawing.Point(8, 8);
			this.panel.Name = "panel";
			this.panel.Size = new System.Drawing.Size(368, 288);
			this.panel.TabIndex = 2;
			this.panel.Controls.Add(this.birthdayControl);
			// 
			// BirthdaysDialog
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(384, 332);
			this.Controls.Add(this.panel);
			this.Controls.Add(this.animateCheck);
			this.Controls.Add(this.closeBtn);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "BirthdaysDialog";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Birthdays";
			this.ResumeLayout(false);

		}
		#endregion

		private void closeBtn_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}
	}
}
