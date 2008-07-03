DXVistaAeroColor plugin for DesktopX
 (c) 2006 Julien TEMPLIER aka Littleboy
---------------------------------------

The DXVistaAeroColor plugin is a scripting plugin that exposes Vista Glass
color control to scripts.

***************************************************************************
** Installation
***************************************************************************

Extract the DXVistaAeroColor.dll file in your DesktopX/SDPlugin folder.

You can now add the new ability to your objects: in the Object Properties,
under Additional Abilities, click "Add" and select the DXVistaAeroColor plugin.
(should appears as Aero Glass Control).

Your object now has a new ability: Control color intensity and color of glass.

***************************************************************************
** Documentation
***************************************************************************

The plugin works by making four new functions available for use in your scripts:
	- AeroColor.SetDefaultColor(id)
	- AeroColor.GetDefaultColor(id, alpha, red, green, blue)
	- AeroColor.SetColorization(alpha, red, green, blue, transparencyEnabled)
	- AeroColor.GetColorization(alpha, red, green, blue, transparencyEnabled)
	

SetDefaultColor(id)
-------------------

Set one of the 8 default Windows colors themes.

id should be one of the following:
 0 : Aero
 1 : Graphite
 2 : Blue
 3 : Teal
 4 : Red
 5 : Orange
 6 : Pink
 7 : Frost
 
 
GetDefaultColor(id, alpha, red, green, blue)
--------------------------------------------

Get one of the 8 default Windows colors themes

id should between 0 and 7 (see above).
After the call, the four last parameters will contain the values corresponding
to the color theme.

If id is invalid, the parameters will be all be 0.


AeroColor.SetColorization(alpha, red, green, blue, transparencyEnabled)
-----------------------------------------------------------------------

Set the aero glass color and transparency.

Each value is between 0 and 255. Transparency should be TRUE or FALSE


AeroColor.GetColorization(alpha, red, green, blue, transparencyEnabled)
-----------------------------------------------------------------------

Get the current aero glass color and transparency.

After the call, each parameter will contain the values corresponding
to the current color theme.

Each value is between 0 and 255. Transparency will be TRUE or FALSE


***************************************************************************
** Changelog
***************************************************************************

Version 1.0.1: First released version

***************************************************************************
** Licence
***************************************************************************
This work is licensed under the Creative Commons
Attribution-ShareAlike License. To view a copy of this license, visit
http://creativecommons.org/licenses/by-sa/2.0/ or send a letter to
Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.
***************************************************************************