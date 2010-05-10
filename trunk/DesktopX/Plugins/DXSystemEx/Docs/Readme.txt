DXSystemEx plugin for DesktopX
 Copyright (c) 2009-2010 Julien Templier
-------------------------------------------------------------------------------------------------------

The DXSystemEx plugin is a scripting plugin that exposes extended system information to DesktopX plugins

*******************************************************************************************************
** Installation
*******************************************************************************************************

Extract the DXSystemEx.dll file in your DesktopX/SDPlugin folder.

You can now add the new ability to your objects: in the Object Properties,
under Additional Abilities, click "Add" and select the DXSystemEx plugin.
(should appears as Extended System Control).

Full source code is available here: http://code.google.com/p/threeoaks/

*******************************************************************************************************
** Documentation
*******************************************************************************************************

-------
Archive
-------
(WIP - only handles zip and does not have any progress callbacks)

Properties & Methods
	- GetArchive()

	Archive Object
      - InputFolder
      - Path
	  - FileName
	  - Password 
      - Count
	  - Items
	  - Type
	  - SupportedExtensions
	  - Create(filePath, type)
	  - Open(filePath)
	  - AddFile(filename)
	  - ExistsFile(filename)
	  - ExtractFile(filename, outputDirectory)
	  - Extract(outputDirectory)
	  - Close()
	  - IsArchive(filePath)
	
	ArchiveItem object
	  - Name
	  - IsDirectory
	  - CompressedSize 

--------
Download
--------
Properties & Methods
	- StartDownload(id, remoteUrl, localPath)
	- StopDownload(id)

Callbacks
	- SystemEx_OnDownloadFinish(id, status, localPath)
	- SystemEx_OnDownloadProgress(id, completedSize, totalSize)

-----------
Drag & Drop
-----------
Callbacks
	- SystemEx_OnDropText(text)
	- SystemEx_OnDropFiles(files)

--------
Instance
--------
Properties & Methods
	- CommandLine
	- CommandLineArgs
	- IsFirstInstance
	- ExecutableFolder
	- ExecutableName

Callbacks
	- SystemEx_OnNewInstance(commandLineArgs)

-------------------
Monitor information
-------------------
Properties & Methods
  - Monitors
  - NumberOfMonitors
  - GetMonitor(index)

  MonitorInfo object:
   - IsPrimary
   - Left
   - Top
   - Bottom
   - Right

----------
Mousewheel
----------
Callbacks:
	- SystemEx_OnMouseWheel(rotation)
	- SystemEx_OnMButtonDown(x, y)
	- SystemEx_OnMButtonUp(x, y, dragged)

---------
Signature
---------
Properties & Methods
	- VerifySignature(path, signature, type)
	- GetSignature(path, type)

------
Volume
------
Properties & Methods
	- Volume
	- Mute
	- PeakValue (read-only)

Callbacks
	- SystemEx_OnVolumeEvent(volume)
	- SystemEx_OnMuteEvent(isMuted)


*******************************************************************************************************
** Methods, Properties and callbacks details
*******************************************************************************************************

=======================================================================================================
== Archive
=======================================================================================================

Note: The only supported archive type is ARCHIVE_ZIP

SystemEx.GetArchive
-------------------

Get an archive object that can be used to manipulate archive files


Archive.Create(filePath, type)
------------------------------

Create a new archive


Archive.Open(filePath)
----------------------

Open an existing archive file


Archive.AddFile(filename)
-------------------------

Add a new file to the archive


Archive.ExistsFile(filename)
----------------------------

Check if a file exists in the archive


Archive.ExtractFile(filename, outputDirectory)
----------------------------------------------

Extract a single file from the archive


Archive.Extract(outputDirectory)
--------------------------------

Extract the contents of the opened archive to outputDirectory


Archive.Close()
---------------

Close the currently opened archive file


Archive.IsArchive(filePath)
---------------------------

Return true if the archive format is handled, false otherwise


=======================================================================================================
== Download
=======================================================================================================

SystemEx.StartDownload(id, remoteUrl, localPath)
-----------------------------------------------

Download a file to a local path, identified by id

SystemEx.StopDownload(id)
-------------------------

Stop a specific download


SystemEx_OnDownloadFinish(id, status, localPath)
------------------------------------------------

Received when a download has ended. See the list of status codes for more information


SystemEx_OnDownloadProgress(id, completedSize, totalSize)
---------------------------------------------------------

Received on download progress



=======================================================================================================
== Drag & Drop
=======================================================================================================

SystemEx_OnDropText(text)
-------------------------

Gets called when the user drops a text selection on the object


SystemEx_OnDropFiles(files)
---------------------------

Gets called when the user drops one or more files on the object. "files" will be an array of file names

Notes: 
 1. You don't need to set the layer to Accept Drag&Drop in the layer options.
 2. Your object will also be set to accept files. The normal Object_OnDropFiles callback will work as
usual (although provided by the plugin instead of DesktopX). You can also get an array of file names
instead of a |-separated list by implementing the SystemEx_OnDropFiles callback.



=======================================================================================================
== Instance
=======================================================================================================

SystemEx.CommandLine
--------------------

Get the full command line (including the path to the executable and DesktopX-specific arguments)


SystemEx.CommandLineArgs
------------------------

Get an array of command line arguments.
Command line arguments have been cleaned up to remove DesktopX-specific arguments (in the case of single-exe gadgets)

SystemEx.IsFirstInstance
------------------------

Will be True if this is the first instance to run, False otherwise.
It is preferable to check for it at startup and close the gadget accordingly,
as only the first instance will receive a callback message when a new instance is started.


SystemEx_OnNewInstance(commandLineArgs)
---------------------------------------

Gets called when another instance is started. The command line arguments are passed in an array.



=======================================================================================================
== Monitor Information
=======================================================================================================

SystemEx.Monitors
------------------

Gets an array of MonitorInfo objects


SystemEx.GetMonitor(index)
---------------------------

Returns the MonitorInfo for the given monitor


SystemEx.NumberOfMonitors
--------------------------

Gets the number of active monitors on the machine



=======================================================================================================
== Mousewheel
=======================================================================================================

SystemEx_OnMouseWheel(rotation)
----------------------------

When the mouse is over the object and the mousewheel is used, the SystemEx_OnMouseWheel
function will be called with the wheel rotation passed as a parameter.

The wheel rotation will be in number of lines.

Default value: 3
Default value for page scrolling: 10

A positive value indicates that the wheel was rotated forward, away from the user;
a negative value indicates that the wheel was rotated backward, toward the user.


SystemEx_OnMButtonDown(x, y) & SystemEx_OnMButtonUp(x, y, dragged)
---------------------------------------------------

When you middle click on your object, the SystemEx_OnMButtonDown function will be
called. When you release the button, the SystemEx_OnMButtonUp function will be called.

Both functions will have the mouse coordinates relative to your object passed as
parameters.


=======================================================================================================
== Signature
=======================================================================================================

Note: The only type of signature supported at this time is SIGNATURE_SHA1


SystemEx.VerifySignature(path, signature, type)
-----------------------------------------------

Check the signature of the file pointed to by path.


SystemEx.GetSignature(path, type)
---------------------------------

Get the signature of the file pointed to by path.



=======================================================================================================
== Volume
=======================================================================================================

SystemEx_OnVolumeEvent(volume)
------------------------------

When the user change the master volume through the volume mixer or another application
your object callback is called. The volume parameter will contain the current master
volume.

XP Compatibility: never called.


SystemEx_OnMuteEvent(isMuted)
-----------------------------

If the volume is muted, your object callback is called.
isMuted will be True if the volume has been muted, false otherwise.

XP Compatibility: never called.


SystemEx.Volume
---------------

Sets or gets the master volume.

Usage:
 SystemEx.Volume = <volume>
 vol = SystemEx.Volume

<volume> should be between 0 and 100. Any value outside these bounds will be capped.


SystemEx.Mute
-------------

Mute or Unmute the audio stream

Usage:
 SystemEx.Mute = True
 isMuted = SystemEx.Mute

Mute can take two values: True and False.
If Mute is True the audio stream is muted, otherwise it is not muted.


SystemEx.PeakValue
------------------

This is a read-only property. It allows you to get the peak level value for the
currently playing sample.

Usage:
	level = SystemEx.PeakValue

level will be between 0 and 100.

XP Compatibility: might not work with some cards, in which case it will always
			      returns 100.
			      It reads from the waveout device, so it won't work when
			      reading from a CD for example.

*******************************************************************************************************
** Changelog
*******************************************************************************************************

Version 1.0 Build xxx:
  + Added Zip/Unzip functions
  + Added HTTP download functions (with progress information)
  + Added drag&drop handling (merged from DXDragDropExtended)
  + Added configuration UI to enable/disable some functions when they are not needed

Version 1.0 Build 240:
  + Renamed to DXSystemEx (plugin namespace is now SystemEx)
  + Added SHA1 signature check
  + Added MouseWheel and Middle button click callbacks (merged from DXMouseWheel)
  + Added Master volume control / Mute / Peak (merged from DXVolumeControl)
  + Added Instance information (merged from DxInstance)

Version 1.0 Build 204: First released version

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

*******************************************************************************************************
** Aknowledgments
*******************************************************************************************************
 Incorporates code from zlib 

 Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler
 
 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.
 2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.
 3. This notice may not be removed or altered from any source distribution.

 Jean-loup Gailly        Mark Adler
 jloup@gzip.org          madler@alumni.caltech.edu

 -------------------------------------------------------------------------------------------------------
 Incorporates code by the Info-ZIP group, further modified by Hans Dietrich and Lucian Wischik

 This is version 2002-Feb-16 of the Info-ZIP copyright and license. The 
 definitive version of this document should be available at 
 ftp:ftp.info-zip.org/pub/infozip/license.html indefinitely.
 
 Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.

 For the purposes of this copyright and license, "Info-ZIP" is defined as
 the following set of individuals:

   Mark Adler, John Bush, Karl Davis, Harald Denker, Jean-Michel Dubois,
   Jean-loup Gailly, Hunter Goatley, Ian Gorman, Chris Herborth, Dirk Haase,
   Greg Hartwig, Robert Heath, Jonathan Hudson, Paul Kienitz, 
   David Kirschbaum, Johnny Lee, Onno van der Linden, Igor Mandrichenko, 
   Steve P. Miller, Sergio Monesi, Keith Owens, George Petrov, Greg Roelofs, 
   Kai Uwe Rommel, Steve Salisbury, Dave Smith, Christian Spieler, 
   Antoine Verheijen, Paul von Behren, Rich Wales, Mike White

 This software is provided "as is", without warranty of any kind, express
 or implied.  In no event shall Info-ZIP or its contributors be held liable
 for any direct, indirect, incidental, special or consequential damages
 arising out of the use of or inability to use this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

    1. Redistributions of source code must retain the above copyright notice,
       definition, disclaimer, and this list of conditions.

    2. Redistributions in binary form (compiled executables) must reproduce 
       the above copyright notice, definition, disclaimer, and this list of 
       conditions in documentation and/or other materials provided with the 
       distribution. The sole exception to this condition is redistribution 
       of a standard UnZipSFX binary as part of a self-extracting archive; 
       that is permitted without inclusion of this license, as long as the 
       normal UnZipSFX banner has not been removed from the binary or disabled.

    3. Altered versions--including, but not limited to, ports to new 
       operating systems, existing ports with new graphical interfaces, and 
       dynamic, shared, or static library versions--must be plainly marked 
       as such and must not be misrepresented as being the original source.  
       Such altered versions also must not be misrepresented as being 
       Info-ZIP releases--including, but not limited to, labeling of the 
       altered versions with the names "Info-ZIP" (or any variation thereof, 
       including, but not limited to, different capitalizations), 
       "Pocket UnZip", "WiZ" or "MacZip" without the explicit permission of 
       Info-ZIP.  Such altered versions are further prohibited from 
       misrepresentative use of the Zip-Bugs or Info-ZIP e-mail addresses or 
       of the Info-ZIP URL(s).

    4. Info-ZIP retains the right to use the names "Info-ZIP", "Zip", "UnZip",
       "UnZipSFX", "WiZ", "Pocket UnZip", "Pocket Zip", and "MacZip" for its 
       own source and binary releases.
