DXVolumeControl plugin for DesktopX
 Copyright 2007-2008, Julien TEMPLIER aka Littleboy
---------------------------------------

The DXVolumeControl plugin adds support for master volume control on
Windows Vista.

It has a basic XP compatibility layer so that you can use it in your objects
without making two different versions. On XP, it will only support the
volume, mute and peak value properties. The callback functions will never be called.

Full source code is available here: http://code.google.com/p/threeoaks/

***************************************************************************
** Installation
***************************************************************************

Extract the DXVolumeControl.dll file in your DesktopX/SDPlugin folder.

You can now add the new ability to your objects: in the Object Properties,
under Additional Abilities, click "Add" and select the DXVolumeControl plugin
(should appears as Vista Volume Control).

Your object now has a new ability: Vista Master Volume Support.

***************************************************************************
** Documentation
***************************************************************************

The plugin works by making two new callback functions available for use in
your scripts:
	- VistaVolume_OnVolumeEvent(volume)
	- VistaVolume_OnMuteEvent(isMuted)
	
In addition to these two callbacks, three new properties allow the modification
of the master volume, muting state and the monitoring of the peak level:
	- VistaVolume.Volume
	- VistaVolume.Mute
	- VistaVolume.PeakValue (read-only)


VistaVolume_OnVolumeEvent(volume)
---------------------------------

When the user change the master volume through the volume mixer or another application
your object callback is called. The volume parameter will contain the current master
volume.

XP Compatibility: never called.


VistaVolume_OnMuteEvent(isMuted)
---------------------------------

If the volume is muted, your object callback is called. 
isMuted will be True if the volume has been muted, false otherwise.

XP Compatibility: never called.


VistaVolume.Volume
------------------

Sets or gets the master volume.

Usage: 
 VistaVolume.Volume = <volume>
 vol = VistaVolume.Volume

<volume> should be between 0 and 100. Any value outside these bounds will be capped.


VistaVolume.Mute
----------------

Mute or Unmute the audio stream

Usage: 
 VistaVolume.Mute = True
 isMuted = VistaVolume.Mute

Mute can take two values: True and False.
If Mute is True the audio stream is muted, otherwise it is not muted.


VistaVolume.PeakValue
---------------------

This is a read-only property. It allows you to get the peak level value for the
currently playing sample.

Usage:
	level = VistaVolume.PeakValue
	
level will be between 0 and 100.

XP Compatibility: might not work with some cards, in which case it will always
			      returns 100.
			      It reads from the waveout device, so it won't work when
			      reading from a CD for example.

***************************************************************************
** Changelog
***************************************************************************

Version 1.2.4: Fixed bug with VistaVolume.Mute not muting audio correctly

Version 1.2.3: Added basic XP compatibility (volume/mute properties only)
			   Fixed crash bug on XP when loading the plugin

Version 1.1.2: Added muting state support (both property and callback)
               Added peak value support (read-only property)

Version 1.0.1: First version (not released)

***************************************************************************
** Licence
***************************************************************************
This work is licensed under the Creative Commons
Attribution-ShareAlike License. To view a copy of this license, visit
http://creativecommons.org/licenses/by-sa/2.0/ or send a letter to
Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.
***************************************************************************