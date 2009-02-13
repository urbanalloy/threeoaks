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
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace ObjectDockSDK
{

	/// <summary>
	///		The docklet class encapsulates the native ObjectDock SDK calls
	/// </summary>
    [Guid("25D7A317-8AE2-4d3c-B7A4-02FDF094EF8B")]
	public class Docklet
	{

		private DOCKLET_STATIC_DATA data;
		private readonly DockletWindowWrapper _window;
		private Control _control;

		/// <summary>
		///		Create a new instance of the docklet class
		/// </summary>
		/// <param name="data">The static data for this docklet</param>
		public Docklet(DOCKLET_STATIC_DATA data)
		{
			this.data = data;
			_window = new DockletWindowWrapper(data.hwndDocklet);
		}

		/***************** Properties *****************/

		#region Windows & Menus

		/// <summary>
		/// Gets the docklet Window (to pass at dialog/messagebox creation).
		/// </summary>
		/// <value></value>
		public DockletWindowWrapper Window
		{
			get {
				return _window;
			}
		}

		/// <summary>
		/// Gets or sets the context menu.
		/// </summary>
		/// <value></value>
		public ContextMenu ContextMenu
		{
			set {
				Control.ContextMenu = value;
			}

			get {
				return Control.ContextMenu;
			}
		}

        /// <summary>
        /// Gets or sets the context menu strip.
        /// </summary>
        /// <value>The context menu strip.</value>
        public ContextMenuStrip ContextMenuStrip
        {
            set
            {
                Control.ContextMenuStrip = value;
            }

            get
            {
                return Control.ContextMenuStrip;
            }
        }

		private Control Control
		{
			get {
				if (_control == null) 
				{
					_control = new Control();
					_control.CreateControl();	
				}
				return _control;
			}
		}

		#endregion

		#region Visibility

		/// <summary>
		/// Gets a value indicating whether or not this docklet is currently visible
		/// </summary>
		/// <value>
		/// 	<c>true</c> if the docklet is visible; otherwise, <c>false</c>.
		/// </value>
		public Boolean IsVisible 
		{
			get {
				return Interop.DockletIsVisible(data.hwndDocklet);
			}
		}

		/// <summary>
		/// Retrieves the bounding rectangle of this docklet in screen coordinates.
		/// </summary>
		/// <value></value>
		public Rectangle Rect
		{
			get {
				Rectangle rect;
				Interop.DockletGetRect(data.hwndDocklet, out rect);
				return rect;
			}
		}

		#endregion

		#region Label

		/// <summary>
		/// Gets or sets the current label of this docklet.
		/// </summary>
		/// <value></value>
		public String Label
		{
			get {
				var label = new StringBuilder(256);
				Interop.DockletGetLabel(data.hwndDocklet, label);
				return label.ToString();
			}

			set {
				Interop.DockletSetLabel(data.hwndDocklet, value.ToCharArray());
			}
		}

		#endregion

		#region Image

		/// <summary>
		/// Sets the image for this docklet.
		/// </summary>
		/// <value></value>
		public Image Image
		{
			set {
				SetImageManaged(value, false);                
			}
		}

		/// <summary>
		/// Sets the image overlay for this docklet.
		/// </summary>
		/// <value></value>
		/// <remarks>
		///		In ObjectDock, instead of dealing with Paint messages,
		///		ObjectDock allows you to set 'overlay' image over
		///		the top of the docklet base image. This allows for such things
		///		as a base background image to be constantly loaded, and
		///		then for details to be added on top of this image, for
		///		example a CPU meter on top of a holder, or a mail
		///		notification icon with a number on top of a mail icon
		///		loaded from a file, which will not have to be reloaded
		///		when mail arrives and the overlay image needs to be
		///		changed.
		///	</remarks>
		public Image Overlay
		{
			set {
				SetImageManaged(value, true);
			}
		}

		/// <summary>
		/// Sets the file to use as an image for this docklet
		/// </summary>
		/// <value></value>
		/// <remarks>
		///		Use a relative (or absolute) path of an image file to set as the
		///		image for the specified docklet
		///	</remarks>
		public String ImageFile
		{
			set {
				// Add the docklet folder to a relative path
				if (!Path.IsPathRooted(value))
					value = Path.Combine(data.relativeFolder, value);

				Interop.DockletSetImageFile(data.hwndDocklet, value.ToCharArray());
			}
		}

		#endregion

		#region Folders

		/// <summary>
		/// Gets the relative path of the folder that contains this Docklet's DLL.
		/// </summary>
		/// <value></value>
		/// <remarks>The relative folder returned is the .Net Docklet folder, not the Interop.dll folder</remarks>
		public String RelativeFolder
		{
			get {
				return data.relativeFolder;
			}
		}

		/// <summary>
		/// Gets the relative path of the folder that contains the Interop DLL.
		/// </summary>
		/// <value></value>
		public String InteropFolder
		{
			get {
				return data.interopFolder;
			}
		}

		/// <summary>
		/// Gets the root folder.
		/// </summary>
		/// <value>The absolute location of the ObjectDock root folder</value>
		/// <remarks>
		///		The path returned includes a final backslash.<br />
		///		To turn any relative path given anywhere into an absolute path,
		///		simply copy the relative path you want to make absolute onto
		///		the end of the the path returned by this function.
		/// </remarks>
		public String RootFolder
		{
			get {
				return data.rootFolder;
			}

		}
		
		#endregion

		#region Edges

		/// <summary>
		///		The Edge of the screen the dock is located on
		/// </summary>
		[Guid("BD862EF2-E889-4506-8676-000F1463FBAF")]
		public enum Edge 
		{
			/// <summary>Bottom Edge</summary>
			BOTTOM,
			/// <summary>Top Edge</summary>
			TOP,
			/// <summary>Left Edge</summary>
			LEFT,
			/// <summary>Right Edge</summary>
			RIGHT
		};

		/// <summary>
		///		The alignment of the dock
		/// </summary>
		[Guid("15BDCAE0-5D06-45ba-A1E6-15AA622F1050")]
		public enum Align 
		{
			/// <summary>Left or Top, if dock is vertical</summary>
			LEFT,
			/// <summary>Middle</summary>
			MIDDLE,
			/// <summary>Right or Bottom, if dock is vertical</summary>
			RIGHT
		};

		/// <summary>
		/// Gets or sets the edge of the screen the dock with this docklet is located on
		/// </summary>
		/// <value></value>
		/// <remarks>Only applicable to a normal dock (always BOTTOM for a tabbed dock)</remarks>
		public Edge DockEdge
		{
			get {
				return (Edge)Interop.DockletQueryDockEdge(data.hwndDocklet);
			}

			set {
				Interop.DockletSetDockEdge(data.hwndDocklet, (int)value);
			}
		}


		/// <summary>
		/// Gets or sets the alignment of the dock with this docklet.
		/// </summary>
		/// <value></value>
		/// <remarks>Only applicable to a normal dock (always MIDDLE for a tabbed dock)</remarks>	
		public Align DockAlign
		{
			get {
				return (Align)Interop.DockletQueryDockAlign(data.hwndDocklet);
			}

			set {
				Interop.DockletSetDockAlign(data.hwndDocklet, (int)value);
			}
		}

		#endregion

		/***************** Functions *****************/

		#region Dialogs

		/// <summary>
		///		Shows the standard ObjectDock "Choose Image" dialog template,
		///		and allows you to select an image file.
		/// </summary>
		/// <param name="image">
		///		Specifies the current image and is filled with the image
		///		that the user selects from the "Choose Image" dialog.
		///	</param>
		///	<param name="alternativeRelativeRoot">
		///		If you would only like to show images from a
		///		specific subfolder of ObjectDock's folder, for example
		///		if you only want the user to pick an image from your
		///		docklet's folder, you can pass a string with a relative
		///		path with the folder you want to start browsing from,
		///		for example, a folder obtained by using the
		///		DockletGetRelativeFolder function.<br />
		///		Pass "" if you want the default folder to be used
		///	</param>
		/// <returns>
		///		true if the image was changed.<br />
		///     false if the image was not changed
		/// </returns>
		public Boolean BrowseForImage(ref String image, string alternativeRelativeRoot)
		{
			var img = new StringBuilder(256);
			img.Insert(0, image);
			
			Boolean ret;
			if (alternativeRelativeRoot == "")
				ret = Interop.DockletBrowseForImage(data.hwndDocklet, img, null);
			else
	            ret =  Interop.DockletBrowseForImage(data.hwndDocklet,
													 img,
													 alternativeRelativeRoot.ToCharArray());
			
            // Update image path
            image = img.ToString();

			return ret;
		}

		/// <summary>
		///		Opens up the default configuration dialog for a given dock item,
		///		thus providing a simple way for the user to adjust a docklet's 
		///		title and image.
		/// </summary>
		/// <remarks>
		///		The default configuration dialog configures a plugin's text and image
		///		only, but can be useful as a method of configuring for simple Docklets
		///  </remarks>
		public void DefaultConfigDialog()
		{
			Interop.DockletDefaultConfigDialog(data.hwndDocklet);
		}

		#endregion

		#region Effects


		/// <summary>
		///		Allows a Docklet to lock the mouse effect (e.g. zooming, etc) of its owning dock.
		/// </summary>
		/// <param name="locked">
		///		true to lock the mouse-over effect (zooming, etc) of the dock<br />
		///		false to unlock the mouse-over effect (zooming, etc) of the dock
		/// </param>
		/// <remarks>
		///		The mouse-over effect lock is done via a counter, so that if multiple
		///		Docklets are locking at the same time the effect will stay locked until
		///		the last Docklet unlocks. The counter does not check to make sure one Docklet
		///		is not continually unlocking however, so do not do this as it will interfere
		///		with other docklets<br />																							is not continually unlocking however, so do not do this as it will interfere with other docklets.
		///		Only lock and unlock the mouse effect when needed.
		/// </remarks>
		public void LockMouseEffect(Boolean locked)
		{
			Interop.DockletLockMouseEffect(data.hwndDocklet, locked);
		}

		/// <summary>
		///		Causes the specified Docklet to animate in the dock to try to get the user's attention.
		/// </summary>
		public void DoAttentionAnimation()
		{
			Interop.DockletDoAttentionAnimation(data.hwndDocklet);
		}

        /// <summary>
        ///		Causes the specified Docklet to animate in the dock to try to get the user's attention.
        ///     Same as DoAttentionAnimation in current builds?
        /// </summary>
        public void DoClickAnimation()
        {
            Interop.DockletDoClickAnimation(data.hwndDocklet);
        }

		#endregion

		#region Menu

		/// <summary>
		/// Shows the context menu associated to this docklet
		/// </summary>
		/// <param name="point">The position where to show the context menu</param> 
		/// <remarks>The menu is shown at the location of point + an offset (-5,-25)</remarks>
		public void ShowContextMenu(Point point)
		{
			if (_control != null) 
			{
				_control.Location = Rect.Location;
				_control.ContextMenu.Show(_control, new Point(point.X-3, point.Y-25));
			}
		}

        /// <summary>
        /// Shows the context menu strip associated to this docklet
        /// </summary>
        /// <param name="point">The position where to show the context menu strip</param> 
        /// <remarks>The menu is shown at the location of point + an offset (-5,-25)</remarks>
        public void ShowContextMenuStrip(Point point)
        {
            if (_control != null)
            {
                _control.Location = Rect.Location;
                _control.ContextMenuStrip.Show(_control, new Point(point.X - 3, point.Y - 25));
            }
        }

		#endregion

		#region Drag&Drop
	
		/// <summary>
		///		Get the coordinates where the file(s) were dropped.
		/// </summary>
		/// <param name="hDrop">Pointer passed to the docklet by OnDropFiles</param>
		/// <returns>
		///		The coordinates where the file(s) were dropped.
		/// </returns>
		public Point GetCoordinates(IntPtr hDrop)
		{
			Point pt;
			Interop.DragQueryPoint(hDrop, out pt);

			return pt;
		}

		/// <summary>
		///		Get the names of the dropped file(s).
		/// </summary>
		/// <param name="hDrop">Pointer passed to the docklet by OnDropFiles</param>
		/// <returns>
		///		An array of strings containing the names of the dropped file(s).
		/// </returns>
		public String[] GetFiles(IntPtr hDrop)
		{
			// get the number of files dropped
			int nb = Interop.DragQueryFile(hDrop, 0xFFFFFFFF, null, 0);

			if (nb == 0)
				return null;

			var files = new String[nb];
			for (int i = 0; i < nb; i++) {
				var str = new StringBuilder(256);
				Interop.DragQueryFile(hDrop, (uint)i, str, str.Capacity);
                files[i] = str.ToString();
			}

			return files;
		}

		#endregion

        #region Misc

        /// <summary>
        /// Remove the docklet from the dock
        /// </summary>
        /// <param name="wParam">??</param>
        /// <returns>true if the function succeeded, false otherwise</returns>
        public bool RemoveSelf(IntPtr wParam)
        {
            return Interop.DockletRemoveSelf(data.hwndDocklet, wParam);
        }

        #endregion

        /***************** Private *****************/

		#region Utility

		private void SetImageManaged(Image newImage, Boolean isOverlay)
		{
			// Lock the bitmap's bits
			var bitmap = new Bitmap(newImage);
			var rect = new Rectangle(0,0,newImage.Width, newImage.Height);
			BitmapData bitmapData = bitmap.LockBits(rect,
				ImageLockMode.ReadOnly,
				PixelFormat.Format32bppArgb);


			// bitmapData.scan0 is the address of the first line.
			Interop.DockletSetImageManaged(data.hwndDocklet,
				bitmapData.Scan0,
				newImage.Width,
				newImage.Height,
				isOverlay);
			
			bitmap.UnlockBits(bitmapData);
		}

		#endregion

	}

	/// <summary>
	/// Encapsulates the handle of the unmanaged docklet window
	/// </summary>
    [Guid("1422A953-FA90-464a-A9D9-6266EB4A007E")]
	public class DockletWindowWrapper : IWin32Window
	{
		private readonly IntPtr _hwnd;

		/// <summary>
		/// Gets the handle of the unmanaged docklet window
		/// </summary>
		/// <value></value>
		public IntPtr Handle
		{
			get { return _hwnd; }
		}

		/// <summary>
		/// Creates a new <see cref="DockletWindowWrapper"/> instance.
		/// </summary>
		/// <param name="handle">Handle of the unmanaged docklet window</param>
		public DockletWindowWrapper(IntPtr handle)
		{
			_hwnd = handle;
		}	
	}

}
// #EOF