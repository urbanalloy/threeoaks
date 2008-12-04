'///////////////////////////////////////////////////////////////////////////////////////////////
'//
'// Dreidel Widget
'//
'// Copyright (c) 2008, Julien Templier
'// All rights reserved.
'//
'///////////////////////////////////////////////////////////////////////////////////////////////
'// * $LastChangedRevision$
'// * $LastChangedDate$
'// * $LastChangedBy$
'///////////////////////////////////////////////////////////////////////////////////////////////

Option Explicit

Dim DreidelController
Dim IniReader

'Called when the script is executed
Sub Object_OnScriptEnter	
 
  ' Initialize data
  Dim dataFolder

  dataFolder = Object.Directory 
  'dataFolder = "D:/Sources/Company/threeoaks/trunk/DesktopX/Widgets/Dreidel/Source/"
  
  Set DreidelController = GetObject("script:" & dataFolder & "DreidelController.wsc")
  Set IniReader = GetObject("script:" & dataFolder & "IniFile.wsc")
  
  DreidelController.Init Object, DesktopX, System, Widget, IniReader, dataFolder
  
  ' Initialize widget preferences
  DreidelController.InitPreferences()
  DreidelController.LoadPreferences()
  
End Sub

Sub Object_OnLButtonUp(x, y, dragged) 
  DreidelController.OnClick()
End Sub

Sub Widget_OnPreferencesChange()
    DreidelController.OnPreferencesChange()
End Sub


Sub Object_OnScriptExit	

End Sub