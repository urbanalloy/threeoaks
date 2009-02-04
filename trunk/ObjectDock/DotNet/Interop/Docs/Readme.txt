.NET Interop Docklet for ObjectDock
 (c) 2004-2009 Julien TEMPLIER
---------------------------------------

The .NET Interop docklet allows you to use .NET docklets with ObjectDock.
A SDK is available if you want to build you own plugins.

***************************************************************************
** Changelog
***************************************************************************

 2.1 (Build xx)
    * Added ContextMenuStrip property to Docklet class (patch by Tide)
    * Added check for custom attribute ObjectDockSDK.SDKVersion and refuse to load if the SDK is too old    

 2.0 (Build 81)
    * Sample docklets, runtime and sdk are now distributed in the same package.
    * Recompiled for .Net 2.0
    * Renamed NetDockletHelper to ObjectDockSDK
    - Fixed Docklet registration (Interop & RegisterHelper)
    - Removed "autorun" code    

 1.0 (Build 35)
	+ Added back FeaturedSkins Sample docklet
	* Misc bugfixes

 0.9 Beta (Build 28):
	* Changed interface definition (see Documentation)
	* AveDesk Compatibility
	+ Drag & Drop helper functions
	+ Ini file reader
	+ Log to console
	- Version showed incorrectly in the docklet selection dialog
	- Stack corruption in selection dialog

 0.9 Alpha:
	First released version


***************************************************************************
** Licence
***************************************************************************
 Redistribution and use in source and binary forms, with or without modification, are
 permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice, this list of
     conditions and the following disclaimer. 
  2. Redistributions in binary form must reproduce the above copyright notice, this list
     of conditions and the following disclaimer in the documentation and/or other materials
     provided with the distribution. 
  3. The name of the author may not be used to endorse or promote products derived from this
     software without specific prior written permission. 

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

***************************************************************************
** Credits/Thanks
***************************************************************************
The Interop icon (Interop.png) is released under the GNU GPL license.
Part of the Crystal SVG Icon Theme (c) 2004 Everaldo
(http://www.everaldo.com/crystal.html)