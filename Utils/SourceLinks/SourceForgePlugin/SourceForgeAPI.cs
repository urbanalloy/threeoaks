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

using WholeTomatoSoftware.SourceLinks;
using System.Collections;
using System;
using System.Net;
using System.Text.RegularExpressions;
using System.Diagnostics;


namespace SourceForgePlugin
{

	/// <summary>
	/// Provides an API to the sourceforge tracker
	/// </summary>
	public class SourceForgeAPI
	{

		private string _projectName = String.Empty;
		public string ProjectName
		{
			get { return _projectName; }
		}

		private string _groupId = String.Empty;
		public string GroupId
		{
			get { return _groupId; }
		}
		
		public bool ShowDetails { get; set; }
		public bool ShowSummary { get; set; }
		public bool ShowLastComment { get; set; }

		private String _lastTrackerId = String.Empty;

		public String Url
		{
			get 
			{
				if (String.IsNullOrEmpty(_groupId))
					return String.Empty;

				return String.Format("http://sourceforge.net/tracker/?func=detail&group_id={0}&atid={1}&aid=%s", _groupId, _lastTrackerId);
			}
		}

		public bool IsConfigured
		{
			get { return !String.IsNullOrEmpty(_projectName) && !String.IsNullOrEmpty(_groupId); }
		}

		public SourceForgeAPI()
		{
			LoadSettings();
		}

		private void LoadSettings()
		{
			try
			{
				Hashtable hashtable = PluginUtils.LoadSettings(typeof(SourceForgePlugin));

				if (hashtable.ContainsKey("ProjectName"))
					_projectName = (String)hashtable["ProjectName"];

				if (hashtable.ContainsKey("GroupId"))
					_groupId = (String)hashtable["GroupId"];

				if (hashtable.ContainsKey("ShowDetails"))
					ShowDetails = (bool)hashtable["ShowDetails"];

				if (hashtable.ContainsKey("ShowSummary"))
					ShowSummary = (bool)hashtable["ShowSummary"];

				if (hashtable.ContainsKey("ShowLastComment"))
					ShowLastComment = (bool)hashtable["ShowLastComment"];
			}
			catch (Exception)
			{
				_projectName = String.Empty;
			}
		}

		public void SaveSettings()
		{
			Hashtable settings = new Hashtable();
			settings["ProjectName"] = _projectName;
			settings["GroupId"] = _groupId;
			settings["ShowDetails"] = ShowDetails;
			settings["ShowSummary"] = ShowSummary;
			settings["ShowLastComment"] = ShowLastComment;

			PluginUtils.SaveSettings(typeof(SourceForgePlugin), settings);
		}

		/// <summary>
		/// Get the project home page and try to get the project id (group_id) from it
		/// </summary>
		/// <param name="projectName">the project name</param>
		/// <returns>true if it succeeds, false otherwise</returns>
		public bool FindGroupId(String projectName)
		{
			try {
				// Get the project homepage
				String page = String.Empty;
				using (var client = new WebClient())
					page = client.DownloadString("http://sourceforge.net/projects/" + projectName);

				if (String.IsNullOrEmpty(page))
					return false;

				// Try getting the group_id value
				string regPattern = "group_id=(?<id>[^\"]*)";
				Regex reg = new Regex(regPattern, RegexOptions.Singleline);
				if (reg.IsMatch(page))
				{
					Match m = reg.Match(page);
					GroupCollection collection = m.Groups;

					// Found our group_id, update the configuration
					_projectName = projectName;
					_groupId = collection["id"].Value;

					return true;
				}
			}
			catch (Exception)
			{
				return false;
			}

			return false;
		}

		/// <summary>
		/// Get information on an entry
		/// </summary>
		/// <param name="entryNumber">the entry number</param>
		/// <returns>a SourceForgeEntry containing the information</returns>
		public SourceForgeEntry GetEntry(int entryNumber)
		{
			SourceForgeEntry entry = new SourceForgeEntry();

			try
			{
				// Search for this entry
				String page = String.Empty;

				using (var client = new WebClient()) 
				{
					page = client.DownloadString(String.Format("http://sourceforge.net/search/index.php?group_id={0}&type_of_search=artifact&q=&artifact_id={1}", _groupId, entryNumber));
					page = TrimPage(page);

					if (String.IsNullOrEmpty(page))
						return null;

					// Try getting the entry data
					string regPattern = "<td class=\"p5\"> (?<priority>[^<]*) </td> <td> (?<id>[^<]*) </td> <td> <a href=\"/tracker/\\?group_id=(?:[0-9]*)&atid=(?<trackerId>[^\"]*)\">(?<tracker>[^<]*)</a> </td> <td class=\"summary\"> <a href=\"(?<url>[^\"]*)\">(?<title>[^<]*)</a> </td> <td> <a (?:[^>]*)>(?<assignee>[^<]*)</a>(?:<a[^>]*><img[^>]*></a>)* </td> <td> <a (?:[^>]*)>(?<submitter>[^<]*)</a> </td> <td> (?<status>[^<]*) </td> <td> (?<opened>[^<]*) </td>";
					Regex reg = new Regex(regPattern, RegexOptions.Singleline);
					if (reg.IsMatch(page))
					{						
						Match m = reg.Match(page);
						GroupCollection collection = m.Groups;

						entry.priority = collection["priority"].Value;
						entry.tracker = collection["tracker"].Value;
						entry.title = collection["title"].Value;
						entry.assignee = collection["assignee"].Value;
						entry.submitter = collection["submitter"].Value;
						entry.status = collection["status"].Value;
						entry.opened = collection["opened"].Value;

						// Save the tracker id for later
						_lastTrackerId = collection["trackerId"].Value;

						// We need to open the bug page and get the details and last comment if necessary
						if (!String.IsNullOrEmpty(collection["url"].Value) && (ShowDetails || ShowLastComment || ShowSummary))
						{
							page = client.DownloadString(String.Format("http://sourceforge.net{0}", collection["url"].Value));
							page = TrimPage(page);
							
							if (String.IsNullOrEmpty(page))
								return entry;

							if (ShowDetails || ShowSummary)
							{
								string detailsPattern = "<label>Details:</label> <p><!-- google_ad_section_start -->(?<details>.*)<!-- google_ad_section_end --></p> <hr noshade=\"noshade\" class=\"divider\" /> <div class=\"yui-u first\"> <label>Submitted:</label> <p>(?<submitter>[^(]*)(?:.*)<label>Resolution:</label> <p>(?<resolution>[^<]*)</p> </div> <div class=\"yui-u\"> <label>Assigned:</label> <p>(?<assignee>[^<]*)</p> <label>Category:</label> <p><!-- google_ad_section_start -->(?<category>[^<]*)<!-- google_ad_section_end --></p> <label>Group:</label> <p>(?<group>[^<]*)</p>";
								Regex regDetails = new Regex(detailsPattern, RegexOptions.Singleline);
								if (regDetails.IsMatch(page))
								{
									Match mDetails = regDetails.Match(page);
									GroupCollection details = mDetails.Groups;

									if (ShowSummary)
										entry.summary = CleanupBRs(details["details"].Value);

									if (ShowDetails)
									{
										entry.submitterName = details["submitter"].Value;
										entry.resolution = details["resolution"].Value;
										entry.assigneeName = details["assignee"].Value;
										entry.category = details["category"].Value;
										entry.group = details["group"].Value;
									}
								}
							}				

							// Last comment
							if (ShowLastComment) 
							{
								string lastCommentPattern = "<input type=\"hidden\" name=\"artifact_comment(?:.*?)<div class=\"yui-u\"(?:[^>]*)> <p><!-- google_ad_section_start -->(?<comment>[^!]*)<br /> <br /> <br /> <!-- google_ad_section_end --></p>";
								Regex regComment = new Regex(lastCommentPattern, RegexOptions.Singleline);
								if (regComment.IsMatch(page))
								{
									MatchCollection mComments = regComment.Matches(page);
									GroupCollection comments = mComments[0].Groups;

									entry.commentCount = mComments.Count;
									entry.lastComment = CleanupBRs(comments["comment"].Value);
								}
							}							
						}
					}
				}
			}
			catch (Exception)
			{
				// Return whatever we were able to parse
				return entry;
			}

			return entry;
		}

		/// <summary>
		/// Trim all double whitespaces, tabs & newlines
		/// </summary>
		/// <param name="page">the page text to trim</param>
		/// <returns>the trimmed text</returns>
		private static String TrimPage(String page)
		{
			page = Regex.Replace(page, @"\s+", " ");
			page = Regex.Replace(page, @"&nbsp;", " "); 
			
			return page;
		}

		private static String CleanupBRs(String text)
		{
			return text.Replace("<br />", Environment.NewLine);			
		}

		
	}
}