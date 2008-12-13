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
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Resources;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Reflection;

//using Outlook = Microsoft.Office.Interop.Outlook;

namespace BirthdayReminder
{
	/// <summary>
	/// Summary description for BirthdayControl.
	/// </summary>
	public class BirthdayControl : System.Windows.Forms.UserControl
	{
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private Boolean populated;
		private BirthdayReminder.BirthdayData data;
		public BirthdayReminder.BirthdayData Data {
			set {
				if (!populated) {
					data = value;
					PopulateControl();
					populated = true;
				}
			}
		}

		// Control related
		private const int headerSize = 32;
		private const int birthdaySize = 23;
		int y = 0;	// Position


		public BirthdayControl() {}

		public void PopulateControl()
		{
			this.SuspendLayout();
			Boolean someBirthday = false;

			// Calculate size
			int height = 0;
			
			if (data.todayB.Count != 0) {
				height += headerSize + data.todayB.Count * birthdaySize; 		
				AddBirthdays("Today's Birthdays", data.todayB);
				someBirthday = true;
			}

			if (data.thisMonthB.Count != 0) {
				height += headerSize + data.thisMonthB.Count * birthdaySize;
				AddBirthdays("This month's Birthdays", data.thisMonthB);
				someBirthday = true;
			}

			if (data.nextMonthB.Count != 0) {
				height += headerSize + data.nextMonthB.Count * birthdaySize;
				AddBirthdays("Next month's Birthdays", data.nextMonthB);
				someBirthday = true;
			}

			if (!someBirthday) {
				//Console.WriteLine("No Birthday !!");

				Label noBirthday = new Label();
				noBirthday.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Bold, GraphicsUnit.Point, ((Byte)(0)));
				noBirthday.Location = new Point(140,40);
				noBirthday.Size = new Size(100,20);
				noBirthday.Text = "No Birthday !";
				this.Controls.Add(noBirthday);
				height += 70;
			}


			this.BackColor = System.Drawing.SystemColors.Control;
			this.Name = "BirthdayControl";
			this.Size = new System.Drawing.Size(350, height);
			this.ResumeLayout(false);		
		}

		private void AddBirthdays(String header, ArrayList indexes)
		{
			Font labelFont = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Bold, GraphicsUnit.Point, ((Byte)(0)));

			//
			// Header
			//			
			y += 8;
			Label headerLabel = new Label();		
			headerLabel.Font = labelFont;
			headerLabel.Location = new Point(8, y);
			headerLabel.Size = new Size(200, 16);
			headerLabel.Text = header;
			this.Controls.Add(headerLabel);

			// Picture
			y += 16;
			PictureBox picture = new PictureBox();
			//picture.Image = Image.FromFile(data.docklet.RootFolder+data.docklet.RelativeFolder+"//BirthdayLine.png");
			picture.Image = Image.FromStream(Assembly.GetExecutingAssembly().GetManifestResourceStream("BirthdayReminder.Resources.BirthdayLine.png"));
			picture.Location = new Point(8, y);
			picture.Size = new Size(300, 1);
			this.Controls.Add(picture);
			y += 8;

			for (int i = 0; i < indexes.Count; i++)
			{
				if (data.type == (int)BirthdayReminder.BirthdayType.INI) 
				{
					Label label = new Label();
					label.Location = new Point(24, y);
					label.Size = new Size(152, 23);
					label.Text = ((BirthdayReminder.Birthday)data.birthdays[(int)indexes[i]]).name;
					this.Controls.Add(label);
				}
				else 
				{ 
					LinkLabel label = new LinkLabel();
					label.Click +=new EventHandler(Birthday_Click);
					label.LinkColor = Color.FromArgb(((Byte)(128)), ((Byte)(128)), ((Byte)(255)));
					label.Location = new Point(24, y);
					label.Size = new Size(152, 23);
					label.Text = ((BirthdayReminder.Birthday)data.birthdays[(int)indexes[i]]).name;
					this.Controls.Add(label);
				}

				Label birthdayLabel = new Label();
				birthdayLabel.Location = new Point(176, y);
				birthdayLabel.Size = new Size(144, 24);
				DateTime bDate = ((BirthdayReminder.Birthday)data.birthdays[(int)indexes[i]]).date;
				birthdayLabel.Text = "(" + (DateTime.Today.Year - bDate.Year) + " years old - " +
					bDate.Day + "/" + bDate.Month + "/" + bDate.Year + ")";

				this.Controls.Add(birthdayLabel);
				y += birthdaySize;
			}
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

		private static void DisposeObject(object o) 
		{ 
			while (Marshal.ReleaseComObject(o) > 0); 
		} 

		private void Birthday_Click(object sender, EventArgs e)
		{
            /*
			// Open the selected birthday
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
			
			try
			{
				for (int i = 1; i <=items.Count; i++) 
				{
					Outlook.ContactItem contact = (Outlook.ContactItem)items[i];

					// FIXME: BAAADDD
					if(contact.FullName.CompareTo(((LinkLabel)sender).Text) == 0)
						contact.Display(true);
				
					DisposeObject(contact);
					contact = null;
				}	
			}
			catch (Exception)
			{
				// DO NOTHING...
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
	}
}
