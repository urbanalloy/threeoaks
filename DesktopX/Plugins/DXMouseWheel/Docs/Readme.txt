DXMouseWheel plugin for DesktopX
 (c) 2006 Julien TEMPLIER aka Littleboy
---------------------------------------

The DXMouseWheel plugin adds support for the mouse wheel to your DesktopX objects.

***************************************************************************
** Installation
***************************************************************************

Extract the DXMouseWheel.dll file in your DesktopX/SDPlugin folder.

You can now add the new ability to your objects: in the Object Properties,
under Additional Abilities, click "Add" and select the DXMouseWheel plugin
(should appears as MouseWheel Support).

Your object now has a new ability: MouseWheel Support.

***************************************************************************
** Documentation
***************************************************************************

The plugin works by making three new callback functions available for use in your scripts:
	- Wheel_OnMouseWheel(rotation)
	- Wheel_OnMButtonDown(x, y)
	- Wheel_OnMButtonUp(x, y)
	

Wheel_OnMouseWheel(rotation)
----------------------------

When the mouse is over the object and the mousewheel is used, the Wheel_OnMouseWheel
function will be called with the wheel rotation passed as a parameter.

The wheel rotation will be a multiple of WHEEL_DELTA, which is set at 120.
A positive value indicates that the wheel was rotated forward, away from the user;
a negative value indicates that the wheel was rotated backward, toward the user.

For more information on MouseWheel messages, see:
http://msdn2.microsoft.com/en-us/library/ms645617.aspx

Note: If your object has the focus but the mouse is over another window, you will
not get mouse wheel events.

Wheel_OnMButtonDown(x, y) & Wheel_OnMButtonUp(x, y)
---------------------------------------------------

When you middle click on your object, the Wheel_OnMButtonDown function will be
called. When you release the button, the Wheel_OnMButtonUp function will be called.

Both functions will have the mouse coordinates relative to your object passed as
parameters.

For more information:
http://msdn2.microsoft.com/en-us/library/ms645610.aspx
http://msdn2.microsoft.com/en-us/library/ms645611.aspx

***************************************************************************
** Changelog
***************************************************************************

Version 1.2.1: Added middle button click detection.

Version 1.1.0: Now use a global mouse hook to capture mouse wheel rotation
			   even if the object doesn't have "focus".

Version 1.0.1: First released version

***************************************************************************
** Licence
***************************************************************************
This work is licensed under the Creative Commons
Attribution-ShareAlike License. To view a copy of this license, visit
http://creativecommons.org/licenses/by-sa/2.0/ or send a letter to
Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.
***************************************************************************