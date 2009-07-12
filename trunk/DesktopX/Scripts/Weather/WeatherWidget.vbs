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
	Dim query
	Set query = WeatherController.GetQueryObject()
	'query.City = "Jamaica Plain"
	query.AirportCode = "KMQE"
	WeatherController.GetWeather query
End Sub

Sub ClearInfo()
	' Reset position
	DesktopX.Object("DXWeather_Info").Top = 0
	DesktopX.Object("DXWeather_Info").Text = ""
End Sub

Sub AppendInfo(info)
	DesktopX.Object("DXWeather_Info").Text = DesktopX.Object("DXWeather_Info").Text & info & vbNewLine
End Sub

'===========================================================
'== Callbacks
'===========================================================
Sub OnLocations(locationInfo)
	Dim location
	
	ClearInfo()
	
	If (locationInfo.Count = 0) Then
		AppendInfo "No location!"
		Exit Sub
	End If
	
	For Each location In locationInfo.Items
		AppendInfo location.ToString() 
	Next	
End Sub

Sub OnWeather(weatherInfo)
	ClearInfo()
	
	AppendInfo weatherInfo.ToString() 
End Sub

Sub OnAlerts(alertInfo)
	Dim alert
	
	ClearInfo()
	
	If (alertInfo.Count = 0) Then
		AppendInfo "No alerts!"
		Exit Sub
	End If
	
	For Each alert In alertInfo.Items
		AppendInfo alert.ToString() 
	Next	
End Sub

Sub OnForecast(forecastInfo)
	Dim forecast
	
	ClearInfo()
	
	If (forecastInfo.Count = 0) Then
		AppendInfo "No forecast!"
		Exit Sub
	End If
	
	For Each forecast In forecastInfo.Items
		AppendInfo forecast.ToString() 
	Next
End Sub

Sub OnCameras(cameraInfo)
	Dim camera
	
	ClearInfo()
	
	If (cameraInfo.Count = 0) Then
		AppendInfo "No cameras!"
		Exit Sub
	End If
	
	For Each camera In cameraInfo.Items
		AppendInfo camera.ToString() 
	Next
End Sub

Sub OnError(code, value)
	ClearInfo()
	
	AppendInfo code & ": " & value
End Sub




