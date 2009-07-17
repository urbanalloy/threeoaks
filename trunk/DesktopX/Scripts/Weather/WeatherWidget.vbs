Option Explicit

Dim WeatherController
Dim ErrorCode
Dim IniFile

Dim m_providerID

'#include "licences.ini.sample"

Dim LIB_FOLDER
LIB_FOLDER = "D:/Sources/Company/src/trunk/src/DesktopX/DXScriptLibrary/Weather/"

'Called when the script is executed
Sub Object_OnScriptEnter	
	Set WeatherController = Nothing
	Set ErrorCode = Nothing
	Set IniFile = Nothing
	m_providerID = ""

	Object.Visible = False
	
	If (SystemEx.IsFirstInstance) Then
		InitWidget()
	Else
		Widget.Close()
	End If
End Sub

'Called when the script is terminated
Sub Object_OnScriptExit
	Set ErrorCode = Nothing
	Set WeatherController = Nothing
	Set IniFile = Nothing
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

	Set ErrorCode = GetObject("script:" & LIB_FOLDER & "Weather.wsc#WeatherError")
	Set WeatherController = GetObject("script:" & LIB_FOLDER & "Weather.wsc")
	
	'#ifdef DEBUG
	WeatherController.AddProvider GetObject("script:" & LIB_FOLDER & "/Providers/TWC.wsc")	
	WeatherController.AddProvider GetObject("script:" & LIB_FOLDER & "/Providers/NOAA.wsc")		
	WeatherController.AddProvider GetObject("script:" & LIB_FOLDER & "/Providers/WUnderground.wsc")
	'#endif
		
	WeatherController.RegisterCallbacks GetRef("OnLocations"), _
										GetRef("OnWeather"), _
										GetRef("OnAlerts"), _
										GetRef("OnForecast"), _
										GetRef("OnCameras"), _
										GetRef("OnError")
											
	WeatherController.UseMetricSystem = True				' Default is True

	'===========================================================================================================	
	' Init Providers
	DesktopX.ScriptObject("DXWeather_Provider").Control.ResetList()

	If (WeatherController.Providers.Count = 0) Then
		Exit Sub
	End If
	
	Dim name
	For Each name In WeatherController.Providers.Items
		DesktopX.ScriptObject("DXWeather_Provider").Control.AddItem name
	Next
	
	DesktopX.ScriptObject("DXWeather_Provider").Control.Enabled = True
	DesktopX.ScriptObject("DXWeather_Provider").Control.ListIndex = 0
	
	OnSelectProvider 0, ""
End Sub
	
Sub OnSelectProvider(item, value)
	Cleanup()
	
	Dim retCode, keys
	
	' Get the provider ids
	keys = WeatherController.Providers.Keys
	
	m_providerID = keys(item)

	retCode = WeatherController.SetProvider(m_providerID)
	
	If retCode <> ErrorCode.E_OK Then
		AppendInfo "Error setting provider: " & retCode & vbNewLine
		Exit Sub
	End If
	
	' Set license info
	retCode = SetLicenceInfo(m_providerID)
	
	If retCode <> ErrorCode.E_OK Then
		ParseRetCode retCode
		DesktopX.ScriptObject("DXWeather_Query").Control.Enabled = False
	Else
		DesktopX.ScriptObject("DXWeather_Query").Control.Enabled = True
	End If
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
		Case ErrorCode.E_OK ' query accepted and request sent
			AppendInfo "Request Sent!" & vbNewLine
		
		Case ErrorCode.E_NOTIMPLEMENTED ' the chosen provider does not implement this method
			AppendInfo "Method not implemented!"  & vbNewLine	' should not happen with GetWeather obviously!
			
		Case ErrorCode.E_NOTAVAILABLE ' the query you used is not supported by this provider/method (for example, you called GetWeather with a country name only)
			AppendInfo "Query type not available!"  & vbNewLine
			
		Case ErrorCode.E_ERROR ' you forgot to set a provider or your query is empty
			AppendInfo "Error!"  & vbNewLine
			
		Case ErrorCode.E_INVALIDLICENSE
			AppendInfo "Invalid license info (check your licenses.ini)!"  & vbNewLine
			
		Case ErrorCode.E_PARSEERROR
			AppendInfo "Parsing error!" & vbNewLine
	End Select
End Sub

Sub Reset()
	'#ifdef DEBUG
	ResetScript "DXWeather", LIB_FOLDER & "/WeatherWidget.vbs"
	'#else
	'InitWidget()
	'#endif
End Sub

'#ifdef DEBUG
Sub ResetScript(name, script)	
	DesktopX.Object(name).SetScript("")
	DesktopX.Object(name).SetScript(script)	
	
	DesktopX.ScriptObject(name).Object_OnScriptEnter()
End Sub
'#endif

'===========================================================
'== Licences
'===========================================================

Function SetLicenceInfo(id)
	' Get iniFile component
	If (IniFile Is Nothing) Then
		Set IniFile = GetObject("script:" & LIB_FOLDER & "./../File/IniFile.wsc")	
		
		IniFile.FileName = LIB_FOLDER & "/licences.ini"
	End If
	
	IniFile.Section = id	

	SetLicenceInfo = WeatherController.SetLicense(IniFile.getKeyValue("id"), IniFile.getKeyValue("key"))
End Function

'===========================================================
'== Actions
'===========================================================
Sub GetLocations()
	ClearInfo()
	
	Dim query, retCode
	Set query = WeatherController.GetQueryObject()
	query.CustomQueryString = DesktopX.ScriptObject("DXWeather_Query").Control.Text
	retCode = WeatherController.GetLocations(query)
	
	ParseRetCode retCode
End Sub

Sub GetForecast()
	ClearInfo()
	
	Dim query, retCode
	Set query = WeatherController.GetQueryObject()	
	
	If (m_providerID = "WUnderground") Then
		query.AirportCode = DesktopX.ScriptObject("DXWeather_Query").Control.Text
	Else
		query.ID = DesktopX.ScriptObject("DXWeather_Query").Control.Text
	End If
	retCode = WeatherController.GetForecast(query)
	
	ParseRetCode retCode
End Sub

Sub GetWeather()
	ClearInfo()
	
	Dim query, retCode
	Set query = WeatherController.GetQueryObject()	
	If (m_providerID = "WUnderground") Then
		query.AirportCode = DesktopX.ScriptObject("DXWeather_Query").Control.Text
	Else
		query.ID = DesktopX.ScriptObject("DXWeather_Query").Control.Text
	End If
	retCode = WeatherController.GetWeather(query)
	
	ParseRetCode retCode
End Sub

Sub GetAlerts()
	ClearInfo()
	
	Dim query, retCode
	Set query = WeatherController.GetQueryObject()	
	query.CustomQueryString = DesktopX.ScriptObject("DXWeather_Query").Control.Text
	retCode = WeatherController.GetAlerts(query)
	
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
	
	AppendInfo "Number of forecasts: " & forecastInfo.Count & vbNewLine
	
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
	AppendInfo "OnError: " & code & " - " & value
End Sub
