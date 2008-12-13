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
using System.IO;
using System.Collections;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using System.Threading;

//using Outlook = Microsoft.Office.Interop.Outlook;

using ObjectDockSDK;
using ObjectDockSDK.Utils;

namespace BirthdayReminder
{
	/// <summary>
	/// Birthday Reminder Docklet
	/// </summary>
	[Guid("90E76BD7-CF85-49b7-B0D6-40FB9CEA7FF4")]
	public class BirthdayReminder : IDockletInterface
	{
		#region Variables

		public enum BirthdayType 
		{
			INI,
			OUTLOOK
		}

		private static string[] DATA_STRING = { "Birthday INI File" };//,
												//"Outlook Address Book (Experimental)"};

		public struct Birthday
		{
			public String name;
            public DateTime date;
		}

		// Threads
		private Thread showBirthdaysThread;
		private Thread animateThread;

		// Delegates
		public delegate void AnimateChanged(object sender, EventArgs e);


		private const int NO_VALUE = -1;
		private static string KEY_TYPE = "BirthDayType";
		private static string KEY_ANIMATE = "Animate";
		private static string KEY_NAME = "Name";
		private static string KEY_DATE = "Date";

		private static string OVERLAYS_FOLDER = "Overlays//";

		private static string SETTINGS_FOLDER = "Stardock//ObjectDockPlus//Docklets//BirthdayReminder//";
		private static string SETTINGS_FILE = "BirthdayReminder";
 
		private Boolean animate = true;
		private int frames = 0;
		private int sleep = 500;
		private Boolean rewind = false;

		public struct BirthdayData
		{
			public Docklet docklet;
			public int type;			

			public ArrayList birthdays;		// All the birthdays
			public ArrayList todayB;		// indexes of today birthdays
			public ArrayList thisMonthB;	// indexes of this month birthdays
			public ArrayList nextMonthB;	// indexed of next month birthdays

			public BirthdayData(Docklet docklet)
			{
				this.docklet = docklet;
				this.type = NO_VALUE;
				this.birthdays = new ArrayList();
				this.todayB = new ArrayList();
				this.thisMonthB = new ArrayList();
				this.nextMonthB = new ArrayList();
			}
		}

		private BirthdayData dataB;

		#endregion

		public void OnGetInformation(out string name, out string author, out int version, out string notes)
		{
			AssemblyData.GetInformation(out name, out author, out version, out notes);
		}

		public void OnCreate(DOCKLET_STATIC_DATA data, String ini, String iniGroup) 
		{
			//ODConsole.Initialize();	
			
			dataB = new BirthdayData(new Docklet(data));
	
			dataB.docklet.Label = "Birthday Reminder Docklet";
			dataB.docklet.ImageFile = "BirthdayReminder.png";

			#region	Load settings
			IniReader iniReader = new IniReader(dataB.docklet.RootFolder+dataB.docklet.RelativeFolder+"docklet.ini");
			frames = iniReader.ReadInteger("Animate", "Frames", 0);
			sleep = iniReader.ReadInteger("Animate", "Sleep", 500);
			rewind = iniReader.ReadBoolean("Animate", "Rewind", false);

			if (ini != String.Empty && iniGroup != String.Empty) 
			{
				iniReader.Filename = ini;
				dataB.type = iniReader.ReadInteger(iniGroup, KEY_TYPE, NO_VALUE);
				animate = iniReader.ReadBoolean(iniGroup, KEY_ANIMATE, true);
			}
			#endregion

			#region ContextMenu
			ContextMenu menu = new ContextMenu();

			MenuItem menu_view = new MenuItem("View Birthdays", new EventHandler(OnMenuConfigure));
			menu_view.Enabled = false;

			MenuItem menu_type = new MenuItem("Birthday Type");
			foreach (String str in DATA_STRING)	{
				menu_type.MenuItems.Add(new MenuItem(str, new EventHandler(OnSelectType)));
			}

			if (dataB.type != NO_VALUE) 
			{
				menu_type.MenuItems[dataB.type].Checked = true;
				menu_view.Enabled = true;
			}

			menu.MenuItems.Add(menu_type);
			menu.MenuItems.Add(menu_view);

			dataB.docklet.ContextMenu = menu;
			#endregion

			#region Load Birthdays
			if (dataB.type == NO_VALUE) {
				dataB.docklet.Label = "Choose a Birthday Type...";
				return;
			}
			LoadBirthdays();
			CalculateBirthdays();
			UpdateDocklet();
			#endregion
		}

		public void OnSave(String ini, String iniGroup, Boolean isForExport)
		{
			// Save DataType
			IniReader iniReader = new IniReader(ini);
			iniReader.Write(iniGroup, KEY_TYPE, dataB.type);
			iniReader.Write(iniGroup, KEY_ANIMATE, animate);
		}

		public void OnDestroy()	{
			if (animateThread != null &&
				animateThread.IsAlive)
			{
				animateThread.Abort();
				animateThread.Join();
			}	

			if (showBirthdaysThread != null &&
				showBirthdaysThread.IsAlive)
			{
				showBirthdaysThread.Abort();
				showBirthdaysThread.Join();
			}
		}

		public String[] OnExportFiles()
		{
			return new String[] {""};
		}

		public Boolean OnConfigure() 
		{
			if (dataB.type == NO_VALUE)
				return true;

			showBirthdaysThread  = new Thread(new ThreadStart(ShowBirthdays));
			showBirthdaysThread.Start();

			return true;
		}

		public Boolean OnLeftButtonClick(ref Point ptCursor, ref Size sizeDocklet)
		{
			OnConfigure();
			return true;
		}

		public Boolean OnRightButtonClick(ref Point ptCursor, ref Size sizeDocklet)
		{
			dataB.docklet.ShowContextMenu(ptCursor);
			return true;
		}

		public Boolean OnDoubleClick(ref Point ptCursor, ref Size sizeDocklet) { return false; }
		public Boolean OnLeftButtonHeld(ref Point ptCursor, ref Size sizeDocklet) {	return false; }
		public Boolean OnAcceptDropFiles() { return false; }	
		public void OnDropFiles(IntPtr hDrop) {}
		public void OnProcessMessage(IntPtr hwnd, uint uMsg, IntPtr wParam, IntPtr lParam) {}

///////////////////////////////////////////////////////////////////////////////////////////////

		/* ************************************** *
		 *				 Events					  *
		 * ************************************** */
		private void OnMenuConfigure(object sender, EventArgs e)
		{
			OnConfigure();
		}

		private void OnSelectType(object sender, EventArgs e)
		{
			// Set the new info
			if (dataB.type != NO_VALUE)
				((MenuItem)sender).Parent.MenuItems[dataB.type].Checked = false;
			
			((MenuItem)sender).Checked = true;
			// Enable View Birthdays
			dataB.docklet.ContextMenu.MenuItems[1].Enabled = true;

			if (dataB.type != ((MenuItem)sender).Index) {
				dataB.type = ((MenuItem)sender).Index;
				LoadBirthdays();
				CalculateBirthdays();
				UpdateDocklet();
			}
		}

		/* ************************************** *
		 *				Loading					  *
		 * ************************************** */
		private void LoadBirthdays()
		{
			switch (dataB.type)
			{
				case (int)BirthdayType.OUTLOOK:
					LoadOutlook();
					break;

				case (int)BirthdayType.INI:
					LoadIni();
					break;

				default:
				case NO_VALUE:
					break;
			}
		}

		private void LoadOutlook()
		{
			dataB.birthdays.Clear();
            /*
			Outlook.Application outlookApp = new Outlook.Application();

			// Get Namespace
			Outlook.NameSpace outlookNamespace = outlookApp.GetNamespace("mapi");

			// Log on 
			// If an outlook app is already open, it will reuse that session, else
			// it will perform a fresh logon (user needs to enter password as needed)
			outlookNamespace.Logon(SystemInformation.UserName, "", true, true);

			// Get the contacts folder
			Outlook.MAPIFolder folder = outlookNamespace.GetDefaultFolder(Outlook.OlDefaultFolders.olFolderContacts);

			// Get all the contacts from the folder
			Outlook.Items items = folder.Items;

			DateTime invalid = DateTime.Parse("01/01/4501");
			try
			{
				// index starts at 1 (don't ask...)
				for (int i = 1; i <=items.Count; i++) {
					Outlook.ContactItem contact = (Outlook.ContactItem)items[i];

					if (DateTime.Compare(contact.Birthday, invalid) != 0)
					{
						Birthday birthday = new Birthday();

						birthday.name = contact.FullName;
						birthday.date = contact.Birthday;

						dataB.birthdays.Add(birthday);
					}
					DisposeObject(contact);
					contact = null;
				}
			}
			catch (Exception e)
			{
                // DO NOTHING...
				Console.WriteLine(e.ToString());
			}
			finally
			{
				// Log off
				outlookNamespace.Logoff();
				outlookApp.Quit();
				
				// Clean up
				DisposeObject(items);
				DisposeObject(folder);
				DisposeObject(outlookNamespace);
				DisposeObject(outlookApp);
				items = null;
				folder = null;
				outlookNamespace = null;
				outlookApp = null;

				GC.WaitForPendingFinalizers();
				GC.Collect();
			}
           */

		}

		private static void DisposeObject(object o) 
		{ 
			while (Marshal.ReleaseComObject(o) > 0); 
		} 

		private void LoadIni()
		{
			dataB.birthdays.Clear();

			String dir = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) +
						 "//" +
						 SETTINGS_FOLDER;

			String file = dir + SETTINGS_FILE + ".ini";

			// Copy template if file doesn't exist
			if (!File.Exists(file)) 
			{
				Directory.CreateDirectory(dir);
				File.Copy(dataB.docklet.RelativeFolder+"//"+SETTINGS_FILE+".bdi", file);
			}

			IniReader ini = new IniReader(file);
	
			ArrayList sections = ini.GetSectionNames();
			if (sections.Count == 0)
				return;

			foreach(String sec in sections) {
				try 
				{
					Birthday birthday = new Birthday();
					birthday.name = ini.ReadString(sec, KEY_NAME);
					birthday.date = DateTime.Parse(ini.ReadString(sec, KEY_DATE));

					dataB.birthdays.Add(birthday);
				}
				catch (Exception) {}
			}
		}


		/* ************************************** *
		 *				 Others					  *
		 * ************************************** */
		private void CalculateBirthdays()
		{
			// Sort birthdays
			IComparer bComparer = new BirthdayComparer();
			dataB.birthdays.Sort(bComparer);

			dataB.todayB.Clear();
			dataB.thisMonthB.Clear();
			dataB.nextMonthB.Clear();

			DateTime today = DateTime.Today;

			Birthday birthday;
			for (int i = 0; i < dataB.birthdays.Count; i++)
			{
				birthday = (Birthday)dataB.birthdays[i];

				if (birthday.date.Month == today.Month) {
					if (birthday.date.Day == today.Day)
						dataB.todayB.Add(i);
					else
						dataB.thisMonthB.Add(i);
				} else {
					if ((birthday.date.Month == today.Month+1)
				     || (today.Month == 12 && birthday.date.Month == 1)) // dec -> janv
						dataB.nextMonthB.Add(i);
				}

			}
		}

		private void UpdateDocklet()
		{
			if (dataB.todayB.Count == 0) 
			{
				dataB.docklet.Label = "No Birthday today!";
				dataB.docklet.Overlay = CreateOverlay();
				if (animateThread != null && animateThread.IsAlive)
				{
					animateThread.Abort();
					animateThread.Join();
				}	
			} 
			else 
			{
				dataB.docklet.Label = dataB.todayB.Count + " people have their Birthday today!";
				dataB.docklet.Overlay = CreateOverlay();
				if (animate) {
					animateThread  = new Thread(new ThreadStart(AnimateDocklet));
					animateThread.Start();
				}
			}
			
		}

		private Image CreateOverlay()
		{
			Bitmap overlay = new Bitmap(128,128);
			if (dataB.todayB.Count == 0)
				return overlay;

			Graphics graphics = Graphics.FromImage(overlay);

			String num = dataB.todayB.Count.ToString();

			// Print background
			int background = num.Length;
			if (background > 3)	background = 3;
			Image back = Image.FromFile(dataB.docklet.RelativeFolder+OVERLAYS_FOLDER+"b"+background+".png");
			graphics.DrawImage(back, 128 - back.Width, 128 - back.Height);

			int x = 94;
			if (num.Length == 1) x = 92;
			for (int i = 0; i < num.Length; i++)
			{
                graphics.DrawImage(Image.FromFile(dataB.docklet.RelativeFolder+OVERLAYS_FOLDER+num[num.Length-(i+1)]+".png"),
								   x-18*i,80);              
			}

			return overlay;
		}

		/* ************************************** *
		 *				 Threads				  *
		 * ************************************** */
		private void AnimateDocklet()
		{
			try 
			{
				ArrayList images = new ArrayList();
				string path = dataB.docklet.RootFolder + dataB.docklet.RelativeFolder+"//Animate//";

				for (int i = 0; i < frames; i++)
					images.Add(Image.FromFile(path+(i+1)+".png"));

				int index = 0;
				Boolean direction = true; // true = normal ; false = rewind
				while (true) {		
                    dataB.docklet.Image = (Image)images[index];
					Thread.Sleep(sleep);

					if (rewind) {
						if (direction) 
						{
							if (index < frames-1)
								index++;
							else {
								direction = false;
								index--;
							}
						} else {	
							if (index > 0)
								index--;
							else {
								direction = true;
								index++;
							}
						}
					} else {
						if (index < frames-1)
							index++;
						else 
							index = 0;
					}
				}
			}
			catch(ThreadAbortException) {
				// Aborting thread
				dataB.docklet.Image = Image.FromFile(dataB.docklet.RootFolder + dataB.docklet.RelativeFolder + "BirthdayReminder.png");
			}
                        
		}

		private void ShowBirthdays()
		{
			try 
			{
				BirthdaysDialog dialog = new BirthdaysDialog(dataB, new AnimateChanged(this.AnimateChangedDelegate), animate);
				dialog.ShowDialog(dataB.docklet.Window);
			}
			catch(ThreadAbortException) 
			{
				// Aborting thread
			}

		}

		private void AnimateChangedDelegate(object sender, EventArgs e)
		{
			animate = ((CheckBox)sender).Checked;
			if (dataB.todayB.Count == 0)
				return;

			if (!animate)
			{
				if (animateThread != null && animateThread.IsAlive)
				{
					animateThread.Abort();
					animateThread.Join();
				}	
			} else {
				if (animateThread == null || !animateThread.IsAlive)
				{
					animateThread  = new Thread(new ThreadStart(AnimateDocklet));
					animateThread.Start();
				}	
			}
		}
	}

///////////////////////////////////////////////////////////////////////////////////////////////
	
	internal class BirthdayComparer : IComparer  
	{
		int IComparer.Compare( Object x, Object y )  
		{
			DateTime b1 = ((BirthdayReminder.Birthday)x).date;
			DateTime b2 = ((BirthdayReminder.Birthday)y).date;

			b2 = b2.AddYears(b1.Year-b2.Year);

			return DateTime.Compare(b1,b2);
		}
	}

}
