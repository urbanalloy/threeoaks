DXSystemEx plugin for DesktopX
 Copyright (c) 2009, Julien TEMPLIER
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

Mousewheel
----------

Callbacks:
	- SystemEx_OnMouseWheel(rotation)
	- SystemEx_OnMButtonDown(x, y)
	- SystemEx_OnMButtonUp(x, y, dragged)


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
   
   
Volume
------

Properties & Methods
	- Volume
	- Mute
	- PeakValue (read-only)

Callbacks
	- SystemEx_OnVolumeEvent(volume)
	- SystemEx_OnMuteEvent(isMuted)
	
	
Misc
---------

Properties & Methods
	- VerifySignature(path, signature, type)
	
	
*******************************************************************************************************

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

*******************************************************************************************************

SystemEx.Monitors
------------------

Gets an array of MonitorInfo objects


SystemEx.GetMonitor(index)
---------------------------

Returns the MonitorInfo for the given monitor


SystemEx.NumberOfMonitors
--------------------------

Gets the number of active monitors on the machine

*******************************************************************************************************

SystemEx_OnVolumeEvent(volume)
---------------------------------

When the user change the master volume through the volume mixer or another application
your object callback is called. The volume parameter will contain the current master
volume.

XP Compatibility: never called.


SystemEx_OnMuteEvent(isMuted)
---------------------------------

If the volume is muted, your object callback is called. 
isMuted will be True if the volume has been muted, false otherwise.

XP Compatibility: never called.


SystemEx.Volume
------------------

Sets or gets the master volume.

Usage: 
 SystemEx.Volume = <volume>
 vol = SystemEx.Volume

<volume> should be between 0 and 100. Any value outside these bounds will be capped.


SystemEx.Mute
----------------

Mute or Unmute the audio stream

Usage: 
 SystemEx.Mute = True
 isMuted = SystemEx.Mute

Mute can take two values: True and False.
If Mute is True the audio stream is muted, otherwise it is not muted.


SystemEx.PeakValue
---------------------

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
		      
SystemEx.VerifySignature(path, signature, type)
-----------------------------------------------

Check the signature of the file pointed to by path.

The only type of signature supported at this time is SIGNATURE_SHA1

*******************************************************************************************************
** Changelog
*******************************************************************************************************

Version 1.0 Build 228:
  + Renamed to DXSystemEx (plugin namespace is now SystemEx)
  + Added SHA1 signature check
  + Added MouseWheel and Middle button click callbacks (merged from DXMouseWheel)
  + Added Master volume control / Mute / Peak (merged from DXVolumeControl)

Version 1.0 Build 204: First released version

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