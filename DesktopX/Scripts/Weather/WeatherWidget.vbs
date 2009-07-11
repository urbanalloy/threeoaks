Dim WeatherController

Dim LIB_FOLDER
LIB_FOLDER = "D:/Sources/Company/src/trunk/src/DesktopX/DXScriptLibrary/Weather/"

'LIB_FOLDER = Object.Directory


'Called when the script is executed
Sub Object_OnScriptEnter	
	Object.Visible = False
	
	If (SystemEx.IsFirstInstance) Then
		InitWidget()
	Else
		Widget.Close()
	End If
End Sub

'Called when the script is terminated
Sub Object_OnScriptExit

End Sub

'===========================================================
'== Events
'===========================================================
Sub SystemEx_OnMouseWheel(lines)
	Dim top, height, mask
	top = DesktopX.Object("DXWeather_Info").Top
	height = DesktopX.Object("DXWeather_Info").Height
	mask = DesktopX.Object("DXWeather_Mask").Height

	If ((top + lines * 4) >= 0) Then
		DesktopX.Object("DXWeather_Info").Top = 0
		Exit Sub
	End If
	
	If ((top + lines * 4) < (-height + mask)) Then
		DesktopX.Object("DXWeather_Info").Top = -height + mask 
		Exit Sub
	End If
	
	DesktopX.Object("DXWeather_Info").Top = top + lines * 4
End Sub

'===========================================================
'== Utils
'===========================================================
Sub InitWidget()
	Object.Visible = True
	Cleanup()

	Set WeatherController = GetObject("script:" & LIB_FOLDER & "Weather.wsc")
	WeatherController.SetProvider "WUnderground"
	
	WeatherController.RegisterCallbacks GetRef("OnLocations"), _
										GetRef("OnWeather"), _
										GetRef("OnAlerts"), _
										GetRef("OnForecast"), _
										GetRef("OnCameras"), _
										GetRef("OnError")
										
	UpdateInfo()
End Sub

Sub Cleanup()
	DesktopX.Object("DXWeather_Info").Text = ""
	DesktopX.Object("DXWeather_Info").Top = 0
End Sub


Sub UpdateInfo()
	Dim location
	'Set location = WeatherController.GetRequestObject()
	Set location =  GetObject("script:" & LIB_FOLDER & "Weather.wsc#WeatherLocation")
	location.Name = "Jamaica Plain"
	WeatherController.GetLocations location
End Sub

Sub ClearInfo()
	' Reset position
	DesktopX.Object("DXWeather_Info").Top = 0
	DesktopX.Object("DXWeather_Info").Text = ""
End Sub

Sub SetInfo(info)
	DesktopX.Object("DXWeather_Info").Text = info
End Sub

'===========================================================
'== Callbacks
'===========================================================
Sub OnLocations(locationInfo)
	MsgBox "Locations"
End Sub

Sub OnWeather(weatherInfo)
	MsgBox "Weather"
End Sub

Sub OnAlerts(alertInfo)
	MsgBox "Alerts"
End Sub

Sub OnForecast(forecastInfo)
	MsgBox "Forecast"
End Sub

Sub OnCameras(cameraInfo)
	MsgBox "Cameras"
End Sub

Sub OnError(errType, errValue)
	ClearInfo()
	
	SetInfo(errType & " - " & errValue)
End Sub




