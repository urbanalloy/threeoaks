DXSystemEx plugin for DesktopX
 (c) 2009 Julien TEMPLIER (Littleboy)
-------------------------------------------------------------------------------------------------------

The DXSystemEx plugin is a scripting plugin that exposes extended system information to DesktopX plugins

*******************************************************************************************************
** Installation
*******************************************************************************************************

Extract the DXSystemEx.dll file in your DesktopX/SDPlugin folder.

You can now add the new ability to your objects: in the Object Properties,
under Additional Abilities, click "Add" and select the DXSystemEx plugin.
(should appears as Extended System Control).

*******************************************************************************************************
** Documentation
*******************************************************************************************************

MonitorInfo object:
 - IsPrimary
 - Left
 - Top
 - Bottom
 - Right



SystemEx.Screens
------------------

Gets an array of MonitorInfo objects


SystemEx.GetScreen(index)
---------------------------

Returns the MonitorInfo for the given screen


SystemEx.NumberOfScreens
--------------------------

Gets the number of active screens on the machine



*******************************************************************************************************
** Changelog
*******************************************************************************************************

Version 1.0 Build xxx:
  + Renamed to DXSystemEX (plugin namespace is now SystemEX)
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