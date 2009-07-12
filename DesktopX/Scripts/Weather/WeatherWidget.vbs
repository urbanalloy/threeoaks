' Constants
Const E_OK 				= 1
Const E_ERROR 			= 0
Const E_NOTAVAILABLE 	= -1
Const E_NOTIMPLEMENTED  = -2
Const E_INVALIDLICENSE  = -3
Const E_PARSEERROR	    = -4


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
	
	Dim retCode

	Set WeatherController = GetObject("script:" & LIB_FOLDER & "Weather.wsc")
		
	WeatherController.RegisterCallbacks GetRef("OnLocations"), _
										GetRef("OnWeather"), _
										GetRef("OnAlerts"), _
										GetRef("OnForecast"), _
										GetRef("OnCameras"), _
										GetRef("OnError")
											
	WeatherController.UseMetricSystem = True				' Default is True
	WeatherController.SetLicense "my_id", "my_key" 			' This will be ignored by the WUnderground provider as it doesn't use a license key
	retCode = WeatherController.SetProvider("WUnderground") ' you can use the WeatherController.Providers property to get a list of <id, display name> pairs
	
	ParseRetCode retCode
End Sub

Sub Cleanup()
	DesktopX.Object("DXWeather_Info").Text = ""
	DesktopX.Object("DXWeather_Info").Top = 0
End Sub

Sub ClearInfo()
	' Reset position
	DesktopX.Object("DXWeather_Info").Top = 0
	DesktopX.Object("DXWeather_Info").Text = ""	
End Sub

Sub AppendInfo(info)
	DesktopX.Object("DXWeather_Info").Text = DesktopX.Object("DXWeather_Info").Text & info & vbNewLine
End Sub

Sub ParseRetCode(code)
	Select Case code
		Case E_OK ' query accepted and request sent
			AppendInfo "OK!" & vbNewLine
		
		Case E_NOTIMPLEMENTED ' the chosen provider does not implement this method
			AppendInfo "Method not implemented!"  & vbNewLine	' should not happen with GetWeather obviously!
			
		Case E_NOTAVAILABLE ' the query you used is not supported by this provider/method (for example, you called GetWeather with a country name only)
			AppendInfo "Query type not available!"  & vbNewLine
			
		Case E_ERROR ' you forgot to set a provider or your query is empty
			AppendInfo "Error!"  & vbNewLine
			
		Case E_INVALIDLICENSE
			AppendInfo "Invalid license info!"  & vbNewLine
			
		Case E_PARSEERROR
			AppendInfo "Parsing error!" & vbNewLine
	End Select
End Sub

'===========================================================
'== Actions
'===========================================================
Sub UpdateInfo()
	ClearInfo()
	
	Dim query, retCode
	Set query = WeatherController.GetQueryObject()	
	query.ID = DesktopX.ScriptObject("DXWeather_Query").Control.Text
	retCode = WeatherController.GetWeather(query)
	
	ParseRetCode retCode
End Sub

Sub GetLocations()
	ClearInfo()
	
	Dim query, retCode
	Set query = WeatherController.GetQueryObject()
	query.CustomQueryString = DesktopX.ScriptObject("DXWeather_Query").Control.Text
	retCode = WeatherController.GetLocations(query)
	
	ParseRetCode retCode
End Sub

'===========================================================
'== Callbacks
'===========================================================
Sub OnLocations(locationInfo)	
	If (locationInfo.Count = 0) Then
		AppendInfo "No location!"
		Exit Sub
	End If
	
	Dim location
	For Each location In locationInfo.Items
		AppendInfo location.ToString() 
	Next	
End Sub

Sub OnWeather(weatherInfo)
	AppendInfo weatherInfo.ToString() 
End Sub

Sub OnAlerts(alertInfo)
	If (alertInfo.Count = 0) Then
		AppendInfo "No alerts!"
		Exit Sub
	End If
	
	Dim alert
	For Each alert In alertInfo.Items
		AppendInfo alert.ToString() 
	Next	
End Sub

Sub OnForecast(forecastInfo)	
	If (forecastInfo.Count = 0) Then
		AppendInfo "No forecast!"
		Exit Sub
	End If
	
	Dim forecast
	For Each forecast In forecastInfo.Items
		AppendInfo forecast.ToString() 
	Next
End Sub

Sub OnCameras(cameraInfo)
	If (cameraInfo.Count = 0) Then
		AppendInfo "No cameras!"
		Exit Sub
	End If
	
	Dim camera
	For Each camera In cameraInfo.Items
		AppendInfo camera.ToString() 
	Next
End Sub

Sub OnError(code, value)
	AppendInfo "OnError: " & code & " - " & valye
End Sub




