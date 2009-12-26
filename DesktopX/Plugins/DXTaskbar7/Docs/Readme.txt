DXTaskbar7 plugin for DesktopX
 Copyright (c) 2009-2010 Julien Templier
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

Tabs management
###############

TabHwnd
-------
Get the handle for that tab (to be used in ConfigureTab)

SetTabsIcon(image)
---------------------
Set a global icon for all the tabs.
  - image: path to a 16x16 image file.

ConfigureTab(name, insertAfter)
-------------------------
Setup the tab name and position.
  - name: title of the tab
  - insertAfter: handle of the tab after which the configured tab should be inserted. Use 0 to insert after the last tab

SetTabActive()
--------------
Set the tab as active and show it in the tab list.

RemoveTab()
-----------
Remove the tab from the list.

Taskbar_OnCloseTab()
--------------------
This function will be called when the user closes a tab in the list.

Taskbar_OnShowTab()
-------------------
This function will be called when the user clicks the preview on the tab list. You should then call Object.OnTop if you want your object to get focus.


ThumbBar
########

SetupButton(id, image, tooltip, flags)
--------------------------------------
Setup a new tab button.
  - id is the internal id (to be passed in the Taskbar_OnButtonClicked callback when the button is clicked)
  - image: path to a 16x16 image file
  - tooltip: the tooltip to shown on mouse-over
  - flags: a list of flags (see DXTaskbarDefines.vbs)

UpdateButton(id, image, tooltip, flags)
---------------------------------------
Update an existing tab button (only after buttons have been added).
  - id is the internal id (to be passed in the Taskbar_OnButtonClicked callback when the button is clicked)
  - image: path to a 16x16 image file
  - tooltip: the tooltip to shown on mouse-over
  - flags: a list of flags (see DXTaskbarDefines.vbs)

AddButtons()
------------
Add the list of buttons to the tab.
A maximum of 7 buttons can be added at one time. After the buttons have been added, you cannot setup new ones. Use the
UpdateButton function to modify existing buttons.

Taskbar_OnButtonClicked(id)
---------------------------
Called when the user clicks on a thumbbar button.
  - id: the button id used in SetupButton and UpdateButton.


Overlay
#######

SetOverlayIcon(path, description)
---------------------------------
Applies an overlay to a taskbar button to indicate application status or a notification to the user.
  - image: path to a 16x16 image file
  - description: an alternative text version, for accessibility purposes

SetProgressState(flag)
----------------------
Sets the type and state of the progress indicator displayed on a taskbar button
  - flag: a progress flag (see DXTaskbarDefines.vbs)

SetProgressValue(completed, total)
----------------------------------
Displays or updates a progress bar hosted in a taskbar button to show the specific percentage completed of the full operation
  - completed: number of steps completed
  - total: total number of steps


Tasks and destinations
######################

AddUserTask(name, path, arguments, icon, iconIndex, workingFolder)
------------------------------------------------------------------
Add a new user tasks (typically static links)
  - name: name of the task
  - path: path to the program to execute
  - arguments: arguments to the program
  - icon: path to .ico file to use as an icon
  - iconIndex: index of the icon to use in the .ico file (typically 0 if you only have 1 icon type)
  - workingFolder: working folder when executing the program

AddSeparator(category)
----------------------
Add a separator
  - category: the category to add a separator to. To add a separator to the tasks, use “Tasks” as the category.

CommitList()
------------
Declares that the Jump List is complete and ready for display

AbortList()
-----------
Discontinues a Jump List building session without committing any changes


*******************************************************************************************************
** Changelog
*******************************************************************************************************

Version 1.0 Build 302
  * Added Taskbar_OnShowTab() callback to allow objects to get focus when a taskbar preview is clicked

Version 1.0 Build 231:
  * Implemented Tasks

Version 1.0 Build 205:
  * Added parameter checking in most functions
  * Fix for Vista and XP: all functions calls are ignored on those systems but are still available to scripts, so you should not get any script error

Version 1.0 Build 198: First released version

*******************************************************************************************************
** License
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