///////////////////////////////////////////////////////////////////////////////////////////////
//
// DotNetDocklets : bringing .NET to ObjectDock
//
// Copyright (c) 2004-2009, Julien Templier
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

namespace ObjectDockSDK
{

	/// <summary>
	///		Contains static docklet data
	/// </summary>
	/// <remarks>Do not use directly.</remarks>
	/// <exclude />
    [Guid("BC48BAB4-19D4-4741-901D-AA0FD88DC14A")]
	public struct DOCKLET_STATIC_DATA 
	{
		/// <summary>
		/// Handle to the window that owns this docklet and therefore identifies the docklet to the host.
		/// </summary>
		public IntPtr hwndDocklet;
		/// <summary>
		/// Handle to the instance of this docklet DLL.
		/// </summary>
		public IntPtr hInstance;
		/// <summary>
		/// Absolute location of the ObjectDock root folder
		/// </summary>
		public string rootFolder;
		/// <summary>
		/// Relative path of the folder that contains the Interop DLL.
		/// </summary>
		public string interopFolder;
		/// <summary>
		/// Relative path of the folder that contains this Docklet's DLL
		/// </summary>
		public string relativeFolder;
	}

	
	/// <summary>
	///		.Net Docklets should implement this interface
	/// </summary>
    [Guid("0AD45A28-F965-421d-A434-7BB746CC2A30")]
	public interface IDockletInterface
	{
		/// <summary>
		///		This function is called when host is looking through a folder of Docklet DLLs
		///		and wants to retrieve information about this specific Docklet DLL.
		/// </summary>
		/// <param name="name">Name of the Docklet</param>
		/// <param name="author">Author of the Docklet</param>
		/// <param name="version">
		///		Version of the Docklet<br />
		///		The version of the Docklet is determined by taking the value placed into
		///		version and dividing by 100. Thus, version = 150; says that this is v1.5
		///		of a Docklet
		///	</param>
		/// <param name="notes">
		///		More detailed description (including any side notes) of the Docklet
		///	</param>
		void OnGetInformation(out String name, out String author, out int version, out String notes);
					
		/// <summary>
		///		This function is called every time a docklet from this dll is created.
		///		Provides the handle of the window the docklet will be represented by,
		///	    along with the instance of the docklet's dll. If the plugin is being
		///	    loaded for the first time, szIni and szIniGroup will be NULL, however
		///	    otherwise they will specify the ini file and ini section the docklet
		///	    should load its individual options from.	   
		/// </summary>
		/// <param name="data">
		///		The data structure containing info about the docklet
		///	</param>
		/// <param name="ini">
		///		Absolute path of an INI file which the plugin should use to load options from.<br />
		///		Can be 0. If 0, then the plugin is a new plugin and default options should be loaded
		///	</param>
		/// <param name="iniGroup">
		///		The INI Section of an INI file which the plugin should use to load options from.<br />
		///		Can be 0. If 0, then the plugin is a new plugin and default options should be loaded.
		/// </param>
		void OnCreate(DOCKLET_STATIC_DATA data, String ini, String iniGroup);

		/// <summary>
		///		 Sent to a plugin when it needs to save its individual properties.
		///		 Since multiple of the same plugins can be in existence	at any given
		///		 time, the plugin should save options that are specific to this instance
		///		 of the plugin to the INI file and section given in szIni and szIniGroup
		///		 and not elsewhere.
		///	</summary>
		/// <param name="ini">
		///		Absolute path of an INI file which the plugin should use to save its
		///		options to for this specific plugin instance.
		///	</param>
		/// <param name="iniGroup">
		///		 The INI Section of an INI file which the plugin should use to save options to.
		///	</param>
		/// <param name="isForExport">
		///		 Specifies whether or not this call to OnSave is the result of the docklet
		///		 being packaged for distribution. While insignificant for most plugins, if
		///		 true plugins storing passwords and other such personal should not save this data.
		/// </param>
		void OnSave(String ini, String iniGroup, Boolean isForExport);

		/// <summary>
		///		Sent right before a plugin is finished being destroyed. The plugin should take
		///		this opportunity to do cleaning up operations it requires.
		/// </summary>
		void OnDestroy();

		/// <summary>
		///		Sent to the docklet when the docklet is to be packaged up as part of a theme
		///		to redistribute.
		/// </summary>
		///	<returns>
		///		The relative paths of the files which this docklet depends upon,
		///		and which should be included if the plugin is packaged up for redistribution.
		///	</returns>
		///	<remarks>Paths are relative to the .Net Docklet directory</remarks>
        String[] OnExportFiles();

		/// <summary>
		///		Called by the host when the dock item that represents this docklet is clicked.
		///		If you choose to implement this function, you should return True to indicate
		///		you processed the message.
		/// </summary>
		/// <param name="ptCursor">
		///		The position of the cursor at the time of the event, in coordinates relative
		///		to the top-left of the docklet image.
		/// </param>
		/// <param name="sizeDocklet">The size of the docklet image at the time of the event.</param>
		/// <returns>
		///		Return true to signify that this event was acted upon.<br />
		///		Return false or do not implement to signify that this event was NOT acted upon.<br />
		///		In this case, for OnLeftButtonClick, no default action will be taken
		/// </returns>
		Boolean OnLeftButtonClick(ref Point ptCursor, ref Size sizeDocklet);

		/// <summary>
		///		Called by the host when the dock item that represents this docklet is double
		///		left-clicked. If you choose to implement this function, you should return
		///		true to indicate you processed the message.
		/// </summary>
		/// <param name="ptCursor">
		///		The position of the cursor at the time of the event, in coordinates relative
		///		to the top-left of the docklet image
		///	</param>
		/// <param name="sizeDocklet">The size of the docklet image at the time of the event.</param>
		/// <returns>
		///		Return true to signify that this event was acted upon.<br />
		///		Return false or do not implement to signify that this event was NOT acted upon.<br />
		///		In this case, for OnDoubleClick, no default action will be taken.<br />
		/// </returns>
		Boolean OnDoubleClick(ref Point ptCursor, ref Size sizeDocklet);

		/// <summary>
		///		Called by the host when the dock item that represents this docklet was either
		///		control+clicked, or had the mouse button held down on it. Typically a plugin
		///		will use this message to display an extended/advanced menu if applicable..
		///     If you choose to implement this function, you should return true to indicate
		///     you processed the message
		/// </summary>
		/// <param name="ptCursor">
		///		The position of the cursor at the time of the event, in coordinates relative
		///		to the top-left of the docklet image
		/// </param>
		/// <param name="sizeDocklet">The size of the docklet image at the time of the event.</param>
		/// <returns>
		///		Return true to signify that this event was acted upon.<br />
		///		Return false or do not implement to signify that this event was NOT acted upon.<br />
		///     In this case, for OnLeftButtonHeld, no default action will be taken.<br />
		/// </returns>
		Boolean OnLeftButtonHeld(ref Point ptCursor, ref Size sizeDocklet);

		/// <summary>
		///		Called by the host when the dock item that represents this docklet is
		///		right-clicked. If you choose to implement this function, you should return
		///		true to indicate you processed the message.<br />
		///		If you do not implement this function or return FALSE, ObjectDock will, as
		///		a default handler, display a menu allowing the user to configure the dock item.
		/// </summary>
		/// <param name="ptCursor">
		///		The position of the cursor at the time of the event, in coordinates relative
		///		to the top-left of the docklet image.
		/// </param>
		/// <param name="sizeDocklet">The size of the docklet image at the time of the event.</param>
		/// <returns>
		///		Return true to signify that this event was acted upon.<br />
		///		Return false or do not implement to signify that this event was NOT acted upon.<br />
		///		In this case, for OnRightButtonClick, the default action will be to display
		///		a simple menu giving the user the opportunity to bring up an options dialog
		///		for the dock item. (This menu item will invoke OnCommand if selected by the user)
		///	</returns>
		Boolean OnRightButtonClick(ref Point ptCursor, ref Size sizeDocklet);

		/// <summary>
		///		Called by the host in the event OnRightButtonClick returned FALSE or was not
		///		implemented, and it was selected to configure the dock item. If you do not
		///		implement this function or return false, ObjectDock will, as a default handler,
		///		will open a basic configuration dialog allowing the dock item's text and image
		///		to be changed. To override this functionality, implement it and return true.
		///	</summary>
		///	<returns>
		///	    Return true to signify that this event was acted upon.<br />
		///	    Return false or do not implement to signify that this event was NOT acted upon.<br />
		///	    In this case, for OnConfigure, a default configuration dialog will be shown,
		///	    which will allow the user to change the docklet's caption and image.
		///	</returns>
		Boolean OnConfigure();

		/// <summary>
		///		Gives the opportunity to a docklet to express its ability to accept dropped
		///		files. Implementing this function and returning TRUE will allow for the
		///		possibility for files to be dropped onto this dock item, and thus a call
		///		to OnDropFiles.
		/// </summary>
		/// <returns>
		///		Return true to signify that this docklet can accept files dropped onto it<br />
		///		Return false or do not implement to signify that this docklet does not accept
		///		dropped files. (Default)
		/// </returns>
		/// <remarks>Only works for normal docks (not tabbed).</remarks>
		Boolean OnAcceptDropFiles();

		/// <summary>
		///		Called by the host when a file has been dropped onto this docklet. The files
		///		being dropped are listed in the HDROP object passed.
		/// </summary>
		/// <param name="hDrop">
		///		 An HDROP object that represents the file(s) dropped onto the docklet item.
		/// </param>
		/// <remarks>
		///		The HDROP structure is freed by the Interop (no need to call DragFinish).<br /><br />
		///		Only works for normal docks (not tabbed).
		/// </remarks>
		void OnDropFiles(IntPtr hDrop);

		/// <summary>
		///		 The window passed on OnCreate which represents and owns the docklet is the
		///		 same window that is visible and displaying the image. While it is
		///		 recommended that you do not try to manually change the appearance or
		///		 position of this window due to this, you can watch this window's messages
		///		 and trap any that are important to you, such as timer messages that you can
		///		 set on the window. If implemented, all messages that are processed by the
		///		 window are passed to this function.
		/// </summary>
		/// <param name="hwnd">
		///		The handle to the window that owns this docklet and therefore identifies
		///		the docklet to the host. (Handle to the window that received the message)
		/// </param>
		/// <param name="uMsg">Specifies the received message</param>
		/// <param name="wParam">
		///		Specifies additional message information.<br />
		///		The content of this parameter depends on the value of the uMsg parameter.
		/// </param>
		/// <param name="lParam">
		///		Specifies additional message information.<br />
		///		The content of this parameter depends on the value of the uMsg parameter.
		/// </param>
		void OnProcessMessage(IntPtr hwnd, uint uMsg, IntPtr wParam, IntPtr lParam);
	}
}
// #EOF