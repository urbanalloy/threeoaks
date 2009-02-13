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
using System.Text;
using System.Drawing;
using System.Runtime.InteropServices;

namespace ObjectDockSDK
{

    [Guid("D619FA7D-D311-4dc8-A599-531506FF4802")]
	internal class Interop
	{
		/////////////////////////
		//     Docklet SDK     //
		/////////////////////////

		[DllImport("Interop.dll")]
		public static extern Boolean DockletIsVisible(IntPtr hwndDocklet);

		[DllImport("Interop.dll")]
		public static extern Boolean DockletGetRect(IntPtr hwndDocklet, out Rectangle rcDocklet);

		[DllImport("Interop.dll")]
		public static extern int DockletGetLabel(IntPtr hwndDocklet, StringBuilder szLabel);

		[DllImport("Interop.dll", CharSet=CharSet.Ansi)]
		public static extern void DockletSetLabel(IntPtr hwndDocklet, char[] szLabel);

		[DllImport("Interop.dll", CharSet=CharSet.Ansi)]
		public static extern void DockletSetImageFile(IntPtr hwndDocklet, char[] szImage);

		[DllImport("Interop.dll", CharSet=CharSet.Ansi)]
		public static extern Boolean DockletBrowseForImage(IntPtr hwndParent,
														   StringBuilder szImage,
														   char[] szAlternateRelativeRoot);

		[DllImport("Interop.dll")]
		public static extern void DockletLockMouseEffect(IntPtr hwndDocklet, Boolean bLock);

		[DllImport("Interop.dll")]
		public static extern void DockletDoAttentionAnimation(IntPtr hwndDocklet);

        [DllImport("Interop.dll")]
        public static extern void DockletDoClickAnimation(IntPtr hwndDocklet);

		[DllImport("Interop.dll")]
		public static extern void DockletDefaultConfigDialog(IntPtr hwndDocklet);

		[DllImport("Interop.dll")]
		public static extern int DockletQueryDockEdge(IntPtr hwndDocklet);

		[DllImport("Interop.dll")]
		public static extern int DockletQueryDockAlign(IntPtr hwndDocklet);

		[DllImport("Interop.dll")]
		public static extern Boolean DockletSetDockEdge(IntPtr hwndDocklet, int iNewEdge);

		[DllImport("Interop.dll")]
		public static extern Boolean DockletSetDockAlign(IntPtr hwndDocklet, int iNewAlign);

        [DllImport("Interop.dll")]
        public static extern Boolean DockletRemoveSelf(IntPtr hwndDocklet, IntPtr wParam);
        
		/////////////////////////
		//        Image        //
		/////////////////////////
		
		// Call into a "proxy" function, as you cannot pass a System.Drawing.Image to unmanaged code
		[DllImport("Interop.dll")]
		public static extern void DockletSetImageManaged(IntPtr hwndDocklet,
														 IntPtr newImage,
														 int width,
														 int height,
														 Boolean bOverlay);

		
		/////////////////////////
		//     Drag & Drop     //
		/////////////////////////
		[DllImport("Shell32.dll")]
		public static extern Boolean DragQueryPoint(IntPtr hDrop, out Point lppt);

		[DllImport("Shell32.dll")]
		public static extern int DragQueryFile(IntPtr hDrop, uint iFile, StringBuilder lpszFile, int cch);


		/////////////////////////
		//        Utils        //
		/////////////////////////
		[DllImport("user32.dll")]
		public static extern IntPtr FindWindowEx(IntPtr hwndParent, IntPtr hwndChildAfter, string lpszClass, string lpszWindow);

		[DllImport("user32.dll")]
		public static extern IntPtr SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);
	}
}
// #EOF