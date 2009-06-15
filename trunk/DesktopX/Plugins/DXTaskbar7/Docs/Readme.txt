DXTaskbar7 plugin for DesktopX
 (c) 2009 Julien TEMPLIER (Littleboy)
-------------------------------------------------------------------------------------------------------

The DXTaskbar7 plugin is a scripting plugin that exposes Windows 7 taskbar
API to scripts.

*******************************************************************************************************
** Installation
*******************************************************************************************************

Extract the DXTaskbar7.dll file in your DesktopX/SDPlugin folder.

You can now add the new ability to your objects: in the Object Properties,
under Additional Abilities, click "Add" and select the DXTaskbar7 plugin.
(should appears as Extended Taskbar Support).

*******************************************************************************************************
** Documentation
*******************************************************************************************************

SetTabsIcon(iconPath)
---------------------
	
Set a global icon for all the tabs.
iconPath is the path to a 16x16 image file.

ConfigureTab(name, after)
-------------------------

Setup the tab name and position
after is the tab hwnd of the tab after which the configured tab should be inserted

SetTabActive()
--------------

Set the tab as active and show it in the tab list

RemoveTab()
-----------

Remove the tab from the list


SetupButton(id, image, tooltip, flags)
--------------------------------------

UpdateButton(id, image, tooltip, flags)
---------------------------------------

AddButtons()
------------
	
SetOverlayIcon(path, description)
---------------------------------

Applies an overlay to a taskbar button to indicate application status or a notification to the user

SetProgressState(flag)
----------------------

Sets the type and state of the progress indicator displayed on a taskbar button

SetProgressValue(completed, total)
----------------------------------

Displays or updates a progress bar hosted in a taskbar button to show the specific percentage completed of the full operation


*******************************************************************************************************
** Changelog
*******************************************************************************************************

Version 1.0 Build 198: First released version

*******************************************************************************************************
** Licence
*******************************************************************************************************
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
*******************************************************************************************************