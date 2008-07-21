Flurry for Windows
Ported by Matt Ginzton
Updated by Julien Templier
---------------------------------------------------------------------------

***************************************************************************
** Credits
***************************************************************************

- This is a port to Windows of Flurry, a beautiful screen saver for
  Mac OS X written by Calum Robinson (http://homepage.mac.com/calumr/).
- Calum credits the original Windows version, Seraphim, by Brian Wade
  (brianwade@va.prestige.net). I can't find any trace of this any more.

***************************************************************************
** Instructions
***************************************************************************

- Copy the Flurry screensaver file (Flurry.scr) to your Windows directory.
  Use the Screen Saver tab of the Desktop control panel to select and
  configure Flurry.
  
- You can also right click the Flurry.scr file and choose Install


Note: this is an OpenGL screensaver and some Intel driver disable hardware
acceleration for OpenGL screensavers, rendering them extremely slow.
You might try to rename the file to Flurry.sCr as a workaround.

***************************************************************************
** Changelog
***************************************************************************

==== Release 1.3 ====

Version 1.3.1 Build 41 (07/21/08)  
	- Added per-monitor flurries
		* Separated global preset and per-monitor preset
		* Simple UI for assigning flurries to monitors
	- Added flurry editor
		* Presets are saved directly after creation/modification. Deletion is effective when closing the main dialog.
	- Bug fixed
		* Preset names were not stored correctly in registry
		* Fixed deletion of preset (UI was not updated correctly)
		* Crash if Flurry wasn't installed before on the computer

Version 1.3.0 Build 26 (07/10/08)    
	- UI Changes
	  * Moved credits to About window
	  * Added UI for shrink percentage & FPS indicator (only in single-buffer mode)
	  * Added UI for Block & Whiteout modes	
	- Editor
	  * Added new/edit/delete buttons
	  * Added Editor dialog (not yet functional)
	- Refactoring of code
	  * separated settings in a class (preparing for per-monitor flurries)
	  * Removed some global variables
	  * cleanup/rename: all classes are now in the Flurry namespace
	  * Updated Spec class (copy constructor, misc updates) in preparation for editor
	- Bug fixed
	  * Preview was not showing the selected preset correctly	  

==== Release 1.2 ====

Version 1.2.1.8 (07/02/08)
	- Use ScrnSave.Lib from VS2005 as the latest version has a dependency on the Vista
	  version of user32.dll
	- Added dialog icon
	- Corrected version & build info retrieval
	- Added theme manifest
	- New icon
	- Merged Readme & Release documents

Version 1.2.0.4 (02/13/05)
	- Fixes errors when reading from registry if the keys don't exist
	- Removed deprecated function calls
	- Added new About Box.
	- Builds 1-3: Developer builds
 
	Known bugs:
		- Hitting Test too many times triggers bad behavior on my NVidia card.
		- Double buffering and multimon interact poorly (slow flip).
		- The screen saver config applet might crash on ATI cards, though
		  the screen saver should function fine if invoked standalone (/c or /s).
		  Note that other problems reported by users with Radeon cards seem to
		  go away when using the newest Catalyst drivers.


==== Release 1.1 ====

Version 1.1.1.11 (8/19/2003)
	- Fixes crash when using the preview in the Screen Saver tab of Display Properties
	  with some ATI Radeon video cards/drivers.

Version 1.1.0.10 (8/10/2003)
	- Tries to automatically throttle the distance between frames on slower graphics cards,
	  so if the rendering isn't keeping up, it gets slower instead of darker
	- "Shrink by %" setting, editable in registry, allows you to use only part of the screen
	  for Flurry (another way to speed it up if it's too slow)
	- Other timing related fixes
	- May work better if your desktop is configured for 16 bit color depth


==== Release 1.0 ====

Version 1.0.0.9 (6/3/2003)
	- Test button in configuration mode invokes screensaver with current, unsaved settings
	- Freakshow modes added
	- Per-monitor visual support added (no UI yet; must edit registry)

Version 1.0.0.8 (6/2/2003)
	- Read visuals from registry, support custom visuals (no UI yet)

Version 1.0.0.7 (5/29/2003)
	- Allow selection of visual in UI (predefined visuals only)
	- Fix multimon bug if primary monitor doesn't start at (0, 0)

Version 1.0.0.6 (5/29/2003)
	- In multimon mode, each flurry draws independently (but they're all the same visual)
	- Support visuals other than Classic (must edit registry to select)

Version 1.0.0.5 (5/23/2003)
	- Multimon support (whole desktop, primary monitor, each monitor but all the same)
	- FPS indicator support

Version 1.0.0.4 (5/20/2003)
	- Stable, no visual configuration options
	- Previous builds were developer builds only.

***************************************************************************
** Updates
***************************************************************************

- Check out julien.wincustomize.com for updates
- Updates may be available at http://www.maddogsw.com/flurry/

Source code is available in the subversion repository here: 
http://code.google.com/p/threeoaks/

***************************************************************************
** License
***************************************************************************

Copyright (c) 2003, Matt Ginzton
Portions copyright (c) 2002, Calum Robinson
Portions copyright (c) 2005-2008, Julien Templier
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

o Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
o Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
o Neither the name of the author nor the names of its contributors may
  be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Enjoy!
