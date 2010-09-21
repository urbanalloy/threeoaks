///////////////////////////////////////////////////////////////////////////////////////////////
//
// SourceForgePlugin - A SourceLinks plugin for the SourceForge bugtracker
//
// Copyright (c) 2010, Julien Templier
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
using System.Windows.Forms;
using System.Reflection;

namespace SourceForgePlugin {

	public partial class SourceForgeConfigDialog : Form	{

		private SourceForgeAPI _api;

		public SourceForgeConfigDialog(SourceForgeAPI api) 
		{
			InitializeComponent();

			_api = api;

			// Update version label
			var version = Assembly.GetExecutingAssembly().GetName().Version;
			label_version.Text = String.Format("Version {0}.{1} Build {2}", version.Major, version.Minor, version.Build);			

			UpdateFields();
		}

		private void UpdateFields() 
		{
			textBox_name.Text = _api.ProjectName;
			textBox_ID.Text = _api.GroupId;

			checkBox_details.Checked = _api.ShowDetails;
			checkBox_summary.Checked = _api.ShowSummary;
			checkBox_lastComment.Checked = _api.ShowLastComment;
		}

		private bool ValidateProject() 
		{
			// Hide error label
			label_error.Visible = false;

			// Store checkboxes status
			_api.ShowDetails = checkBox_details.Checked;
			_api.ShowSummary = checkBox_summary.Checked;
			_api.ShowLastComment = checkBox_lastComment.Checked;

			// If no project name is defined, reset group_id and show error
			if (String.IsNullOrEmpty(textBox_name.Text)) 
			{
				textBox_ID.Text = "";
				label_error.Visible = true;
				return false;
			}

			// If project name has changed or the plugin is not configured, check for the group_id
			if (textBox_name.Text != _api.ProjectName || !_api.IsConfigured)
			{
				if (_api.FindGroupId(textBox_name.Text)) 
				{
					UpdateFields();
					return true;
				}

				label_error.Visible = true;
				return false;
			}

			return true;	
		}

		private void OK_Click(object sender, EventArgs e)
		{
			if (!ValidateProject())
				return;

			// Close the window if the configuration is valid
			if (_api.IsConfigured) 
			{
				base.Close();
			}
		}
	}
}
