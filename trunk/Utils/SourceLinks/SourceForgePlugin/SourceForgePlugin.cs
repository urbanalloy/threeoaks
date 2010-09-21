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

/*
 * To deploy this API extension, build the project, exit Visual Studio, and copy SourceForgePlugin.slp from
 * the output directory to %LocalAppData%\Microsoft\VisualStudio\10.0\Extensions\Whole Tomato Software\SourceLinks\<version>.
 * Restart Visual Studio, go to Tools | Options | SourceLinks, add or edit a profile and select the
 * plug-in from the drop-down list.
 *
 * To easily see the trace messages, compile in Debug configuration and run DebugView,
 * available from http://technet.microsoft.com/en-us/sysinternals/bb896647.aspx.  Be
 * sure to run as Administrator if using Windows Vista.
 */
namespace SourceForgePlugin
{

	/// <summary>
	/// SourceForge Plugin for SourceLinks
	/// </summary>
	internal class SourceForgePlugin : WholeTomatoSoftware.SourceLinks.IPlugin
	{

		private SourceForgeAPI _api;

		public SourceForgePlugin()
		{
			_api = new SourceForgeAPI();
		}

		// Provide a name to appear in the plug-in dropdown in SourceLinks configuration.
		public string Name
		{
			get { return "SourceForge Plugin"; }
		}

		/// <summary>
		/// Provide a URL to open when the user double-clicks marked text.
		/// Use %s as a placeholder for marker text.
		/// This method is called every time the user double-clicks, allowing you
		/// to specify a state-dependent URL if you wish.
		/// </summary>
		public string OverrideUrl
		{
			get	{ return _api.Url; }
		}

		/// <summary>
		/// Checks if the extension has a configuration GUI
		/// </summary>
		/// <returns>Return true if your extension provides a configuration GUI.</returns>
		public bool CanConfigure()
		{
			return true;
		}

		/// <summary>
		/// Implement the configuration GUI as a modal dialog.
		/// </summary>
		public void Configure()
		{
			using (SourceForgeConfigDialog sourceForgeConfigDialog = new SourceForgeConfigDialog(_api))
			{
				sourceForgeConfigDialog.ShowDialog();
				_api.SaveSettings();
			}
		}

		/// <summary>
		/// Assemble a tooltip string using the marker text as input.
		/// </summary>
		/// <param name="markerText">input marker</param>
		/// <returns>the tooltip</returns>
		public string GetTooltip(string markerText)
		{
			if (!_api.IsConfigured)
				return FormatText("<Bold>Plugin not configured!</Bold>");

			// Get the entry info
			String text = String.Empty;

			try
			{
				int entryNumber = Int32.Parse(markerText);
				SourceForgeEntry entry = _api.GetEntry(entryNumber);

				// Check that we get a proper entry
				if (entry == null)
					return FormatText("<Bold>Error retrieving entry!</Bold>");

				text = entry.GetTooltip();

				if (!String.IsNullOrEmpty(text))
					return FormatText(text);
			}
			catch (ArgumentNullException)
			{
				return FormatText("<Bold>Error: Not a valid entry number!</Bold>");
			}
			catch (Exception e)
			{
				return FormatText(String.Format("<Bold>Internal error: {0}</Bold>", e.Message));
			}

			return text;
		}

		private static String FormatText(String text)
		{
			// Add paragraph markers
			text = String.Format("<Paragraph>{0}</Paragraph>", text);

			// Replaced all forms of line breaks
			text = text.Replace(Environment.NewLine, "<LineBreak />");			
			text = text.Replace("\n", "<LineBreak />");
			text = text.Replace("\r", "<LineBreak />");

			// Put inside a FlowDocument
			text = String.Format("<FlowDocument xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\" ColumnWidth=\"140.0\">{0}</FlowDocument>", text);

			return text;
		}
	}

}