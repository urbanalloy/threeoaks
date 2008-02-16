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
using System.Runtime.InteropServices;
using System.Drawing;
using System.Net;
using System.Collections;
using System.IO;
using System.Windows.Forms;
using System.Threading;

using ObjectDockSDK;
using ObjectDockSDK.Utils;

namespace FeaturedSkins
{

	[Guid("9CD67B8B-4C75-4212-A2F2-6D55D770EE2D")]
	public class FeaturedSkins : IDockletInterface
	{      
		private Docklet docklet;

		private static int NO_VALUE = -1;
		
		// Threads
		private Thread showSkinThread;
		private Thread updateThread;

		#region Docklet Data

		private DockletData dockletData;

		private enum ApplicationType
		{
			INVALID,
			INI,
			CSV
		}

		private struct ApplicationData
		{
			public string Name;		 // Skin Name
			public string Author;	 // Author Name

			public Uri UrlDownload;	 // Download URL
			public Uri UrlSkin;		 // Skin URL
			public Uri UrlPreview;	 // Preview URL
			public Uri UrlZoom;		 // Zoom URL

			public string FileName;  // File Name
			public int FileSize;	 // File Size
		}

		private struct Application
		{
			public string name;
			public Uri address;
			public ApplicationType type;
			public ApplicationData data;
		}

		private struct DockletData 
		{
			public ArrayList Applications;
			public int index;				// index of the selected application in the array
			public String cacheDir;			// Name of the temporary directory
		}

		#endregion


		public void OnGetInformation(out string name, out string author, out int version, out string notes)
		{
			AssemblyData.GetInformation(out name, out author, out version, out notes);
		}

		public void OnCreate(DOCKLET_STATIC_DATA data, String ini, String iniGroup) 
		{
			ODConsole.Initialize();

			docklet = new Docklet(data);

			docklet.Label = "Featured Skins Docklet";
			docklet.ImageFile = "FeaturedSkins.png";

			#region Global Settings
			// Initialize docklet data
			dockletData = new DockletData();
			dockletData.Applications = new ArrayList();
			dockletData.index = NO_VALUE; // No application selected

			// Read global settings from ini file
			IniReader iniReader = new IniReader(docklet.RootFolder+docklet.RelativeFolder+"docklet.ini");

			ArrayList apps = iniReader.GetSectionNames();
			foreach (String app in apps) 
			{
				if (app.CompareTo("docklet") == 0)
					continue;

				Application application = new Application();
				// Name
				application.name = app;
				// Address
				application.address = new Uri(iniReader.ReadString(application.name, "address", ""));
				// Type
				string type = iniReader.ReadString(application.name, "type", "");
				switch (type) 
				{
					default:
						application.type = ApplicationType.INVALID;
						break;
					case "ini":
						application.type = ApplicationType.INI;
						break;
					case "csv":
						application.type = ApplicationType.CSV;
						break;
				}
				application.data = new ApplicationData();

				dockletData.Applications.Add(application);
			}
			#endregion

			#region Saved Settings
			if (ini != String.Empty && iniGroup != String.Empty) 
			{
				// Reuse the INIReader
				iniReader.Filename = ini;

				// Read saved app index
				dockletData.index = iniReader.ReadInteger(iniGroup, "AppIndex", NO_VALUE);
				dockletData.cacheDir = iniReader.ReadString(iniGroup, "CacheDir", String.Empty);

				if (dockletData.index > dockletData.Applications.Count)
					dockletData.index  = NO_VALUE;
			}
			#endregion

			#region Context Menu

			ContextMenu menu = new ContextMenu();
			MenuItem appsMenu = new MenuItem("Applications");
			MenuItem refreshMenu = new MenuItem("Refresh");
			refreshMenu.Enabled = false;
			
			foreach (Application app in dockletData.Applications) 
			{
				appsMenu.MenuItems.Add(new MenuItem(app.name, new EventHandler(onSelectApp)));
				if (app.type == ApplicationType.INVALID)
					appsMenu.MenuItems[appsMenu.MenuItems.Count-1].Enabled = false;
			}

			if (dockletData.index != NO_VALUE) 
			{
				appsMenu.MenuItems[dockletData.index].Checked = true;
				refreshMenu.Enabled = true;
			}

			menu.MenuItems.Add(appsMenu);
			menu.MenuItems.Add(refreshMenu);

			// Show the menu
			docklet.ContextMenu = menu;	

			#endregion

			// Get temporary directory for this session
			if (dockletData.cacheDir == String.Empty || dockletData.cacheDir == null) {
				Guid guid = Guid.NewGuid();
				dockletData.cacheDir = guid.ToString();
			}

			// Load Application Data
			if (dockletData.index != NO_VALUE) 
			{
				updateThread  = new Thread(new ThreadStart(UpdateData));
				updateThread.Start();
			}
			
		}


		public void OnSave(String szIni, String szIniGroup, Boolean bIsForExport)
		{
			IniReader iniReader = new IniReader(szIni);
			iniReader.Write(szIniGroup, "AppIndex", dockletData.index);
			iniReader.Write(szIniGroup, "CacheDir", dockletData.cacheDir);
		}

		public void OnDestroy()	{
			if (updateThread != null &&
				updateThread.IsAlive)
			{
				updateThread.Abort();
				updateThread.Join();
			}		

			if (showSkinThread != null &&
				showSkinThread.IsAlive)
			{
				showSkinThread.Abort();
				showSkinThread.Join();
			}
		}

		public String[] OnExportFiles()
		{
			return null;
		}

		
		public Boolean OnConfigure() { return false; }

		public Boolean OnLeftButtonClick(ref Point ptCursor, ref Size sizeDocklet)
		{
			// Wait until download is complete
			if (updateThread.IsAlive)
				return true;

			// Show the zoom image
			showSkinThread  = new Thread(new ThreadStart(ShowSkin));
			showSkinThread.Start();

			return true;
		}

		public Boolean OnDoubleClick(ref Point ptCursor, ref Size sizeDocklet)
		{
			return false;
		}

		public Boolean OnLeftButtonHeld(ref Point ptCursor, ref Size sizeDocklet)
		{
			return false;
		}

		public Boolean OnRightButtonClick(ref Point ptCursor, ref Size sizeDocklet)
		{
			docklet.ShowContextMenu(ptCursor);   
			return true;
		}

		public Boolean OnAcceptDropFiles()
		{
			return false;
		}	

		public void OnDropFiles(IntPtr hDrop){}
		public void OnProcessMessage(IntPtr hwnd, uint uMsg, IntPtr wParam, IntPtr lParam) {}



///////////////////////////////////////////////////////////////////////////////////////////////

		
		private void onSelectApp(object sender, EventArgs e)
		{
			// Stop the current update thread
			if (updateThread != null &&
				updateThread.IsAlive) {
				updateThread.Abort();
				updateThread.Join();
			}	

			// Set the new info
			if (dockletData.index != NO_VALUE)
                ((MenuItem)sender).Parent.MenuItems[dockletData.index].Checked = false;
			
			((MenuItem)sender).Checked = true;
			dockletData.index = ((MenuItem)sender).Index;
			docklet.ContextMenu.MenuItems[1].Enabled = true; // Enable refresh menu			

			// Start a new update thread
			updateThread  = new Thread(new ThreadStart(UpdateData));
			updateThread.Start();
		}

		/// <summary>
		///	Check for modified data and download as appropriate
		/// </summary>
		private void UpdateData()
		{
			try {
				docklet.Label = "Downloading Info...";
				docklet.ImageFile = "FeaturedSkins.png";

				Application app = (Application)dockletData.Applications[dockletData.index];

				if (app.type == ApplicationType.INVALID)
					return;

				// Create Cache directory
				String cache = Path.GetTempPath() + "/FeaturedSkins/" + dockletData.cacheDir + "/" + app.name + "/";
				Directory.CreateDirectory(cache);

				// Download config file
				WebClient webClient = new WebClient();
				webClient.DownloadFile(app.address.ToString(), cache+FileFromURL(app.address));
			
				Console.WriteLine("Finished Downloading file");
				// Read info from the file
                app.data = ReadInfoFromFile(cache+FileFromURL(app.address));

				Console.WriteLine("Finished Reading info from file");
				
				// Download Preview & Zoom Files
				Console.WriteLine("  Downloading Preview...");
				if (app.data.UrlPreview != null &&
					!File.Exists(cache+FileFromURL(app.data.UrlPreview)))
					webClient.DownloadFile(app.data.UrlPreview.ToString(), cache+FileFromURL(app.data.UrlPreview));

				Console.WriteLine("  Downloading Zoom...");
				if (app.data.UrlZoom != null &&
					!File.Exists(cache+FileFromURL(app.data.UrlZoom))) 
				{
					webClient.DownloadFile(app.data.UrlZoom.ToString(), cache+FileFromURL(app.data.UrlZoom));					
				}

				// Set Preview as Icon
				Image img = Image.FromFile(cache+FileFromURL(app.data.UrlPreview));			
				docklet.Image = img;
				docklet.Label = app.data.Name + " By " + app.data.Author;

				// Save data
				dockletData.Applications[dockletData.index] = app;

			}
			catch(WebException e)
			{
				Console.WriteLine("File not found...");
				Console.WriteLine(e);
			}
			catch(ThreadAbortException) 
			{
				Console.WriteLine("Aborting Update thread");
				
			}
			finally {
				Console.WriteLine("Finished Update Thread");
			}
		}

		private ApplicationData ReadInfoFromFile(String file)
		{
			// Populate the current app data
            ApplicationData data = new ApplicationData();

			switch (((Application)dockletData.Applications[dockletData.index]).type)
			{
				case ApplicationType.INI:
					IniReader iniReader = new IniReader(file);
					data.Name = iniReader.ReadString("Featured", "Name", "");
					data.Author = iniReader.ReadString("Featured", "Author", "");
					data.UrlDownload = new Uri(iniReader.ReadString("Featured", "DownloadUrl", ""));
					data.UrlPreview = new Uri(iniReader.ReadString("Featured", "PreviewUrl", ""));
					data.UrlZoom = new Uri(iniReader.ReadString("Featured", "ZoomUrl", ""));
					/////////////////////
					//  Other Entries  //
					/////////////////////					
					try {
						data.UrlSkin = new Uri(iniReader.ReadString("Featured", "SkinUrl", ""));
					} catch (UriFormatException) {}					
					data.FileName = iniReader.ReadString("Featured", "FileName", "");
					data.FileSize = iniReader.ReadInteger("Featured", "FileSize", NO_VALUE);
					/////////////////////
					break;

				case ApplicationType.CSV:
					CSVReader csvReader = new CSVReader(file);
					try {
						// Get the names & values
						string[] names = csvReader.GetCSVLine();
						string[] values = csvReader.GetCSVLine();

						for (int i = 0; i < names.Length; i++)	
						{
							switch(names[i])
							{
								case "AuthorName":
									data.Author = values[i];
									break;
								case "SkinName":
									data.Name = values[i];
									break;
								case "DownloadURL":
									data.UrlDownload =  new Uri(values[i]);
									break;
								case "SkinURL":
									data.UrlSkin = new Uri(values[i]);
									break;
								case "PreviewURL":
									data.UrlPreview =  new Uri(values[i]);
									break;							
								case "FileName":
									data.FileName = values[i];
									break;		
								case "FileSize":
									data.FileSize = int.Parse(values[i]);
									break;
									/////////////////////
									//  Other Entries  //
									/////////////////////
								case "ZoomURL":
									data.UrlZoom =  new Uri(values[i]);
									break;
									/////////////////////	
								default:
									break;
							}
						}															 
					}
					catch (CSVReaderException) {}		
					catch (UriFormatException) {}
					break;

				case ApplicationType.INVALID:
				default:
					break;
			}

			return data;
		}

		private String FileFromURL(Uri url)
		{
			String[] path = url.GetLeftPart(UriPartial.Path).Split('/');
			return path[path.Length-1];
		}

		private void ShowSkin()
		{
			try {
				Application app = (Application)dockletData.Applications[dockletData.index];

				if (app.type == ApplicationType.INVALID)
					return;

				String cache = Path.GetTempPath() + "/FeaturedSkins/" + dockletData.cacheDir + "/" + app.name + "/";
				String pic = "";
				if (app.data.UrlZoom == null)
					pic = cache + FileFromURL(app.data.UrlPreview);
				else
					pic = cache + FileFromURL(app.data.UrlZoom);

				FeaturedDialog dialog = new FeaturedDialog(app.data.UrlSkin, pic);
				
				dialog.ShowDialog(docklet.Window);
			}
			catch(ThreadAbortException) 
			{
				// Aborting thread
			}
		}


	}
}
