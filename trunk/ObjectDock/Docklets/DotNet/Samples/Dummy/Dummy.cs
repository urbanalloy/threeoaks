///////////////////////////////////////////////////////////////////////////////////////////////
//
// Dummy .Net Docklet for ObjectDock
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
using System.Windows.Forms;

using ObjectDockSDK;
using ObjectDockSDK.Utils;

namespace Dummy
{
	/// <summary>
	/// Dummy Docklet for ObjectDock (.NET)
	/// </summary>
	[Guid("847355FE-F9C3-4352-8FE2-FA7F68A9523D")]
	public class Dummy : IDockletInterface
	{
		private Docklet docklet;

		public void OnGetInformation(out string name, out string author, out int version, out string notes)
		{
			AssemblyData.GetInformation(out name, out author, out version, out notes);
		}

		public void OnCreate(DOCKLET_STATIC_DATA data, String ini, String iniGroup) 
		{
			ODConsole.Initialize();

			docklet = new Docklet(data);
	
			docklet.Label = "Dummy Docklet";
			docklet.ImageFile = "Dummy.png";

			#region Context Menu

			ContextMenu menu = new ContextMenu();
			MenuItem config = new MenuItem("Configure", new EventHandler(onMenuConfigure));
	
			MenuItem testmenu = new MenuItem("Test Menu");
            testmenu.MenuItems.Add("Browse For Image", new EventHandler(onBrowseForImage));
			testmenu.MenuItems.Add("Item 1");
			testmenu.MenuItems.Add("Item 2");

			menu.MenuItems.Add(config);
			menu.MenuItems.Add(testmenu);

			docklet.ContextMenu = menu;	

			#endregion
		}	    

	    private void ShowInfo()
		{
			// Label
			Console.WriteLine("Label: " + docklet.Label);
			
			// Visibility
			Console.WriteLine("Visibility: " + docklet.IsVisible + "\n");

			// Rectangle
			Console.WriteLine("Rectangle: " + docklet.Rect.ToString() + "\n");

			// Edge & Align
			Console.WriteLine("Edge: " + docklet.DockEdge + "\n");
			Console.WriteLine("Align: " + docklet.DockAlign + "\n");

		}

		public void OnSave(String ini, String iniGroup, Boolean isForExport) {	}

		public void OnDestroy()	{ }

		public String[] OnExportFiles()
		{
			return new String[] {"Dummy.png",
			 		     "Dummy.dll",
					     "docklet.ini"};
		}

		private void onMenuConfigure(object sender, EventArgs e)
		{
			OnConfigure();
		}

        private void onBrowseForImage(object sender, EventArgs e)
        {
            string image = "Dummy.png";
            bool ret = docklet.BrowseForImage(ref image, "");

            if (ret)
                Console.WriteLine("New Image: " + image + "\n");

        }

		public Boolean OnConfigure() 
		{
			docklet.DoAttentionAnimation();
			ShowInfo();
			docklet.DefaultConfigDialog();

			return true;
		}

		public Boolean OnLeftButtonClick(ref Point ptCursor, ref Size sizeDocklet)
		{
			OnConfigure();
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
			Console.WriteLine("Accept Drop files...");
			return true;
		}	

		public void OnDropFiles(IntPtr hDrop) {
			Console.WriteLine("Dropped file...");
		
			Console.WriteLine(docklet.GetCoordinates(hDrop).ToString());

			String[] files = docklet.GetFiles(hDrop);
			Console.WriteLine(files.Length);
			foreach (string str in files)
				Console.WriteLine(str);
		
		}

		public void OnProcessMessage(IntPtr hwnd, uint uMsg, IntPtr wParam, IntPtr lParam) {}
	}
}
