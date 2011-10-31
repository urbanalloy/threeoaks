'///////////////////////////////////////////////////////////////////////////////////////////////
'//
'// Weather Script Component
'//
'// Copyright (c) 2009-2010, Julien Templier
'// All rights reserved.
'//
'///////////////////////////////////////////////////////////////////////////////////////////////
'//
'// Redistribution and use in source and binary forms, with or without modification, are
'// permitted provided that the following conditions are met:
'// 1. Redistributions of source code must retain the above copyright notice, this list of
'//    conditions and the following disclaimer.
'// 2. Redistributions in binary form must reproduce the above copyright notice, this list
'//    of conditions and the following disclaimer in the documentation and/or other materials
'//    provided with the distribution.
'// 3. The name of the author may not be used to endorse or promote products derived from this
'//    software without specific prior written permission.
'//
'// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
'// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
'// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
'// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
'// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
'// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
'// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
'// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
'// POSSIBILITY OF SUCH DAMAGE.
'//
'///////////////////////////////////////////////////////////////////////////////////////////////

Option Explicit

Dim WeatherController
Dim Constants
Dim IniFile

Dim m_providerID

'#include "licences.ini.sample"

Dim LIB_FOLDER
LIB_FOLDER = "D:/Sources/Company/src/trunk/src/DesktopX/DXScriptLibrary/Weather/"

'Called when the script is executed
Sub Object_OnScriptEnter
	Set WeatherController = Nothing
	Set Constants = Nothing
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
	Set Constants = Nothing
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

	Set Constants = GetObject("script:" & LIB_FOLDER & "Weather.wsc#WeatherConstants")
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

	If retCode <> Constants.E_OK Then
		AppendInfo "Error setting provider: " & retCode & vbNewLine
		Exit Sub
	End If

	' Set license info
	retCode = SetLicenceInfo(m_providerID)

	If retCode <> Constants.E_OK Then
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
		Case Constants.E_OK ' query accepted and request sent
			AppendInfo "Request Sent!" & vbNewLine

		Case Constants.E_NOTIMPLEMENTED ' the chosen provider does not implement this method
			AppendInfo "Method not implemented!"  & vbNewLine	' should not happen with GetWeather obviously!

		Case Constants.E_NOTAVAILABLE ' the query you used is not supported by this provider/method (for example, you called GetWeather with a country name only)
			AppendInfo "Query type not available!"  & vbNewLine

		Case Constants.E_ERROR ' you forgot to set a provider or your query is empty
			AppendInfo "Error!"  & vbNewLine

		Case Constants.E_INVALIDLICENSE
			AppendInfo "Invalid license info (check your licenses.ini)!"  & vbNewLine

		Case Constants.E_PARSEERROR
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

Sub GetCameras()
	ClearInfo()

	Dim query, retCode
	Set query = WeatherController.GetQueryObject()
	query.CustomQueryString = DesktopX.ScriptObject("DXWeather_Query").Control.Text
	retCode = WeatherController.GetCameras(query)

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

	AppendInfo "Number of locations: " & locationInfo.Count & vbNewLine

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

	AppendInfo "Number of cameras: " & cameraInfo.Count & vbNewLine

	Dim camera
	For Each camera In cameraInfo.Items
		AppendInfo camera.ToString()
	Next
End Sub

Sub OnError(code, value)
	AppendInfo "OnError: " & code & " - " & value
End Sub
