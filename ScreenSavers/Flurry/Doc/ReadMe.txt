Flurry for Windows
ported by Matt Ginzton, matt@maddogsw.com
modified by Julien Templier, littleboy22 at gmail dot com
---------------------------------------------------------------------------

***************************************************************************
** Credits
***************************************************************************

- This is a port to Windows of Flurry, a beautiful screen saver for
  Mac OS X written by Calum Robinson (http://homepage.mac.com/calumr/).
- Calum credits the original Windows version, Seraphim, by Brian Wade
  (brianwade@va.prestige.net).  I can't find any trace of this any more.

***************************************************************************
** Instructions
***************************************************************************

- Copy the Flurry screensaver file (Flurry.scr) to your Windows directory.
  Use the Screen Saver tab of the Desktop control panel to select and
  configure Flurry.


***************************************************************************
** Changelog
***************************************************************************

Release 1.2.1: 07/02/08, build 1.2.1.8
 - Use ScrnSave.Lib from VS2005 as the latest version has a dependency on the Vista version of user32.dll
 - Added dialog icon
 - Corrected version & build info retrieval
 - Added theme manifest
 - New icon
 - Merged Readme & Release documents

Release 1.2.0: 02/13/05, build 1.2.0.4

Known bugs:
 - Hitting Test too many times triggers bad behavior on my NVidia card.
 - Double buffering and multimon interact poorly (slow flip).
 - The screen saver config applet might crash on ATI cards, though
   the screen saver should function fine if invoked standalone (/c or /s).
   Note that other problems reported by users with Radeon cards seem to
   go away when using the newest Catalyst drivers.

Build 4 (1.2.0.4): 02/13/05
 - Fixes errors when reading from registry if the keys don't exist
 - Removed deprecated function calls
 - Added new About Box

Builds 1-3: Developer builds

Release 1.1.1: 8/19/03, build 1.1.1.11

Build 11 (1.1.1.11): 8/19/2003
 - Fixes crash when using the preview in the Screen Saver tab of Display
   Properties with some ATI Radeon video cards/drivers.

Release 1.1: 8/10/2003, build 1.1.0.10

Build 10 (1.1.0.10): 8/10/2003
 - Tries to automatically throttle the distance between frames on slower
   graphics cards, so if the rendering isn't keeping up, it gets slower
   instead of darker
 - "Shrink by %" setting, editable in registry, allows you to use only
   part of the screen for Flurry (another way to speed it up if it's too
   slow)
 - Other timing related fixes
 - May work better if your desktop is configured for 16 bit color depth

Build 9 (1.0.0.9): 6/3/2003
 - Test button in configuration mode invokes screensaver with current,
   unsaved settings
 - Freakshow modes added
 - Per-monitor visual support added (no UI yet; must edit registry)

Build 8 (1.0.0.8): 6/2/2003
 - Read visuals from registry, support custom visuals (no UI yet)

Build 7 (1.0.0.7): 5/29/2003
 - Allow selection of visual in UI (predefined visuals only)
 - Fix multimon bug if primary monitor doesn't start at (0, 0)

Build 6 (1.0.0.6): 5/29/2003
 - In multimon mode, each flurry draws independently (but they're all the
   same visual)
 - Support visuals other than Classic (must edit registry to select)

Build 5 (1.0.0.5): 5/23/2003
 - Multimon support (whole desktop, primary monitor, each monitor but all
   the same)
 - FPS indicator support

Release 1.0: 5/20/2003, build 1.0.0.4

Build 4 (1.0.0.4): 5/20/2003
 - Stable, no visual configuration options

Previous builds were developer builds only.

***************************************************************************
** Updates
***************************************************************************

- Check out julien.wincustomize.com for updates
- Updates may be available at http://www.maddogsw.com/flurry/

***************************************************************************
** License
***************************************************************************

Copyright (c) 2003, Matt Ginzton
Portions copyright (c) 2005,2008, Julien Templier
Portions copyright (c) 2002, Calum Robinson
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

- matt@maddogsw.com
- littleboy22 at gmail.com
