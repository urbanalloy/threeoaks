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

using System.Web;
using System;

namespace SourceForgePlugin {

	/// <summary>
	/// Information on a SourceForge tracker entry
	/// </summary>
    public class SourceForgeEntry {

		// Basic fields
		public string priority = string.Empty;
        public string tracker = string.Empty;
        public string title = string.Empty;
		public string assignee = string.Empty;
		public string submitter = string.Empty;
        public string status = string.Empty;
        public string opened = string.Empty;

		// Details
		public string summary = string.Empty;
		public string lastComment = string.Empty;

		public string submitterName = string.Empty;
		public string assigneeName = string.Empty;
		public string resolution = string.Empty;
		public string category = string.Empty;
		public string group = string.Empty;
		public int commentCount = 0;

		public String GetTooltip() {
			String text = String.Empty;

			if (!String.IsNullOrEmpty(title))
				text += "<Span Foreground=\"Navy\"><Bold>Summary:</Bold></Span> " + HttpUtility.HtmlEncode(title);

			if (!String.IsNullOrEmpty(status))
				text += "<LineBreak /><Span Foreground=\"Navy\"><Bold>Status:</Bold></Span> " + HttpUtility.HtmlEncode(status);

			if (!String.IsNullOrEmpty(resolution))
				text += "<LineBreak /><Span Foreground=\"Navy\"><Bold>Resolution:</Bold></Span> " + HttpUtility.HtmlEncode(resolution);

			if (!String.IsNullOrEmpty(category))
				text += "<LineBreak /><Span Foreground=\"Navy\"><Bold>Category:</Bold></Span> " + HttpUtility.HtmlEncode(category);

			if (!String.IsNullOrEmpty(group))
				text += "<LineBreak /><Span Foreground=\"Navy\"><Bold>Group:</Bold></Span> " + HttpUtility.HtmlEncode(group);

			if (!String.IsNullOrEmpty(assignee))
				text += "<LineBreak /><Span Foreground=\"Navy\"><Bold>Assigned To:</Bold></Span> " + HttpUtility.HtmlEncode(!String.IsNullOrEmpty(assigneeName) ? assigneeName : assignee);

			if (!String.IsNullOrEmpty(submitter))
				text += "<LineBreak /><Span Foreground=\"Navy\"><Bold>Submited By:</Bold></Span> " + HttpUtility.HtmlEncode(!String.IsNullOrEmpty(submitterName) ? submitterName : submitter);

			if (!String.IsNullOrEmpty(opened))
				text += "<LineBreak /><Span Foreground=\"Navy\"><Bold>Date Opened:</Bold></Span> " + HttpUtility.HtmlEncode(opened);

			if (!String.IsNullOrEmpty(priority))
				text += "<LineBreak /><Span Foreground=\"Navy\"><Bold>Priority:</Bold></Span> " + HttpUtility.HtmlEncode(priority);

			if (!String.IsNullOrEmpty(tracker))
				text += "<LineBreak /><Span Foreground=\"Navy\"><Bold>Tracker:</Bold></Span> " + HttpUtility.HtmlEncode(tracker);

			if (!String.IsNullOrEmpty(summary))
			{
				// Truncate text if too long
				if (summary.Length > 400)
					summary = summary.Substring(0, 400) + " [...]";

				text += "<LineBreak /><LineBreak /><Span Foreground=\"Navy\"><Bold>Details:</Bold></Span> " + HttpUtility.HtmlEncode(HttpUtility.HtmlDecode(summary));
			}

			if (commentCount != 0 && !String.IsNullOrEmpty(lastComment))
			{
				// Truncate text if too long
				if (lastComment.Length > 400)
					lastComment = lastComment.Substring(0, 400) + " [...]";

				text += "<LineBreak /><LineBreak /><Span Foreground=\"Navy\"><Bold>Last comment (of " + HttpUtility.HtmlEncode(commentCount.ToString()) + "):</Bold></Span> " + HttpUtility.HtmlEncode(HttpUtility.HtmlDecode(lastComment));
			}

			return text;
		}

    }

}
