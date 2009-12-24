DockletX plugin for DesktopX
 Copyright (c) 2006-2009 Julien Templier
--------------------------------------------------------------------------

DockletX is a DesktopX plugin that allows you to use ObjectDock docklets
on your desktop.

***************************************************************************
** Installation
***************************************************************************

Extract the DockletX.dll file in your DesktopX/SDPlugin folder.

You can now add the new ability to your objects: in the Object Properties,
under Additional Abilities, click "Add" and select the DockletX plugin.
(should appears as ObjectDock Docklet Support).

In the configuration, you can set the size for your docklet. By default, the
docklet size will be 128x128.

Select a docklet from the list and press OK. The docklet should load and 
appear in your object.

To interact with the docklet, use CTRL+Click.
To open the default context menu, use CTRL+SHIFT+Right Click.

***************************************************************************
** Troubleshooting/Known bugs
***************************************************************************

PLEASE REPORT ALL BUGS IN THIS THREAD: http://forums.wincustomize.com/339574/
  
  - Weather is not working (the docklet is hardcoded inside ObjectDock)   
  - Export is not working (ie. it doesn't package the docklet with the dxpack - disabled in ObjectDock anyway)   
  - SetLabel/GetLabel update the tooltip rather than the label (not available from the plugin API) 
  
  - Attention animations are not implemented
  
  - Only ObjectDock docklet are supported. RocketDock and Y'z Dock docklets will not work and may crash DesktopX.

***************************************************************************
** Changelog
***************************************************************************

1.0 Build 604: Corrected detection of free version of ObjectDock

1.0 Build 418: First public release

1.0 Build 389: Test version


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
** Aknowledgments
***************************************************************************

 
***************************************************************************
** Credits/Thanks
***************************************************************************
