///////////////////////////////////////////////////////////////////////////////////////////////
//
// DotNetDocklets : bringing .NET to ObjectDock
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

using System.Runtime.InteropServices;
using System;

namespace ObjectDockSDK.Utils
{
	/// <summary>
	///	Notepad log
	/// </summary>
	[Guid("68EC9EA2-22ED-4db5-A443-2A8822C6C622")]
	public class Notepad
	{

		/// <summary>
		///		Send a message to the notepad window
		/// </summary>
		/// <param name="obj">obj.ToString() will be sent to the opened notepad window</param>
		public static void LOG(object obj)
		{
#if LOG
			const int WM_CHAR = 0x0102; 
			IntPtr hwndNotepad = Interop.FindWindowEx(IntPtr.Zero, IntPtr.Zero, "Notepad", null); 
			IntPtr hwndEdit    = Interop.FindWindowEx(hwndNotepad, IntPtr.Zero, "Edit", null); 

			if(hwndNotepad == IntPtr.Zero || hwndEdit == IntPtr.Zero)
				return;

			char[] str = obj.ToString().ToCharArray();
			for (int j = 0; j < str.Length; j++)
				Interop.SendMessage(hwndEdit, WM_CHAR, str[j], 0);
#endif
		}


		// Store the hDrop info into a DataObject
//		public static DataObject GetDataObject(IntPtr hDrop)
//		{
//			return new DataObject();
//
//		}
	}
}
// #EOF