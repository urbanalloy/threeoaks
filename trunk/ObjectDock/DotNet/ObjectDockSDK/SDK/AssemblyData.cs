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
using System.Reflection;
using System.Runtime.InteropServices;

namespace ObjectDockSDK
{
	/// <summary>
	/// Gets the information about this docklet from the Assembly Info
	/// </summary>
    [Guid("0C16326F-E9A1-436a-ABFE-CF2057A6DB89")]
	public class AssemblyData
	{
		/// <summary>
		/// Gets the information about this docklet
		/// </summary>
		/// <param name="name">Name of the docklet</param>
		/// <param name="author">Author of the docklet</param>
		/// <param name="version">Version  of the docklet</param>
		/// <param name="notes">Notes about this docklet</param>
		public static void GetInformation(out string name, out string author, out int version, out string notes)
		{
			name = "";
			author = "";
			notes = "";

			Assembly caller = Assembly.GetCallingAssembly();

			Object[] objArray = caller.GetCustomAttributes(false);
			foreach (Object obj in objArray) {
				// Name
				AssemblyTitleAttribute title = obj as AssemblyTitleAttribute;
				if (title != null) name = title.Title;
				// Author
				AssemblyCopyrightAttribute copyright = obj as AssemblyCopyrightAttribute;
				if (copyright != null) author = copyright.Copyright;
				// Notes
				AssemblyDescriptionAttribute note = obj as AssemblyDescriptionAttribute;
				if (note != null) notes = note.Description;
			}

			Version ver = caller.GetName(false).Version;
			version = ver.Major*100
					+ ver.Minor*10
					+ ver.Build;
		}
	}
}