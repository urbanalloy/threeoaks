Option Explicit

Dim scripts
Dim currentScriptPath

' Test Suite
Const STATUS_INVALID = -1
Const STATUS_FAIL = 0
Const STATUS_SUCCESS = 1
Const STATUS_MANUAL = 2

Dim areTestRunning
Dim test_scripts
Dim current_test_index
Dim test_report
Dim tests_failed
Dim tests_unknown
Dim tests_passed
Dim test_id

' Pin to Desktop
Dim pinned
Dim pLeft
Dim pTop

'Called when the script is executed
Sub Object_OnScriptEnter
	areTestRunning = False
	Set test_report = Nothing
	test_id = Month(Now) & "." & Day(Now) & "." & Year(Now) & " " & Hour(Now) & ":" & Minute(Now) & ":" & Second(Now)

	' Save object position
	pLeft = Object.Left
	pTop = Object.Top

	Set scripts = Nothing
	currentScriptPath = ""
	ResetObject "CanvasObject"
	ResetObject "CanvasObjectJS"
	
	ResetUI
			
	RegisterPreferences
	
	If Widget.Preference("LockPosition").Value = "1" Then
		DesktopX.Object("CC_Icons_Pin").State = "Pinned"
		pinned = True
	Else
		pinned = False
	End If
	
	If (DesktopX.HostType = 2) Then
		Widget.Preference("TestsFolder").Value = "D:/Sources/Company/threeoaks/trunk/DesktopX/Plugins/DXCanvas/Test/TestSuite"
		Widget.Preference("ScriptFolder").Value = "D:/Sources/Company/threeoaks/trunk/DesktopX/Plugins/DXCanvas/Test/"
		
		DesktopX.Object("CC_Title").Text = "Canvas Controller 2 - DEV"
	End If
	
	' Check that the test suite is present and hide/show button
	ShowTestsIcon CheckTestSuitePresent()	
	
	LoadScripts()
End Sub

'Called when the script is terminated
Sub Object_OnScriptExit
	Object.KillTimer 1
	Object.KillTimer 2

 	RemoveCurrentScript()
	Set scripts = Nothing
	Set test_scripts = Nothing
	
	CloseReport True
End Sub

'------------------------------------------------------------
'-- Events & Timers
'------------------------------------------------------------
Sub Object_OnTimer1
	' Kill the timer
	Object.KillTimer 1
	
	' Load all scripts from the current folder
	LoadScriptsInternal()
End Sub

Sub Object_OnTimer2
	' Kill the timer while we are running the test
	Object.KillTimer 2
	
	' Stop running tests if asked
	If (areTestRunning = False) Then
		Exit Sub
	End If
	
	If (current_test_index > test_scripts.Count -1) Then
		CloseReport False		
		ResetUI()
		ShowReport()
		
		Exit Sub
	End If
	
	UpdateTestInfo current_test_index + 1
	
	' Load the test
	Dim path
	path = test_scripts.Item(current_test_index)
	
	If (path = "") Then
		OnAbortTests
	End If
	
	' Remove the currently running script and load the new one
	RemoveCurrentScript()
	LoadScript path
			
	' Register update callback
	If (currentScriptPath = "") Then		
		OnAbortTests
	End If
	 	
	DesktopX.ScriptObject(GetCanvasName(currentScriptPath)).RegisterCallback GetRef("UpdateReport")
	
	' Run the test
	StartScript True
		
	' Set index for next test to be executed
	current_test_index = current_test_index + 1
End Sub	

Sub OnAbortTests
	' Stop tests
	areTestRunning = False
	CloseReport True
	
	ResetUI()
End Sub

'------------------------------------------------------------
'-- Buttons & Misc
'------------------------------------------------------------
Const VK_RETURN = &H0D
Const VK_LEFT = &H25
Const VK_RIGHT = &H27
Sub Object_OnKeyDown(key, flags)
	If key = VK_RETURN Then
		OnApplyScript()
	End If
	
	If key = VK_LEFT Then
		SelectPreviousScript()
	End If
	
	If key = VK_RIGHT Then
		SelectNextScript()
	End If	
End Sub

Sub Object_OnDrag(x, y, newX, newY)
	If pinned Then
		Object.Top = pTop
		Object.Left = pLeft
	End If
End Sub

Sub PinToDesktop()
	pLeft = Object.Left
	pTop = Object.Top
	pinned = Not pinned
	
	If (pinned) Then
		Widget.Preference("LockPosition").Value = "1"
	Else
		Widget.Preference("LockPosition").Value = "0"
	End If
End Sub

Sub ResetUI()
	DesktopX.Object("CC_Title").Text = "Canvas Controller 2"

	DesktopX.Object("CC_Tests_Info").Visible = False
	DesktopX.Object("CC_Invalid").Visible = False
	
	DesktopX.ScriptObject("CC_Select").Control.Enabled = True

	' Restore load object
	DesktopX.Object("CC_Button_OK_Text").Text = "Load"
	DesktopX.ScriptObject("CC_Button_OK").SetCallBack GetRef("OnApplyScript")
End Sub

'------------------------------------------------------------
'-- Tests
'------------------------------------------------------------
Sub GenerateTestResults(unattended)
	If Not CheckTestSuitePresent Then
		Exit Sub
	End If
	
	Dim answer
	If Not unattended Then
		answer = DesktopX.MsgBox("Running the test suite can take a long time. Are you sure you want to continue?" & vbNewLine & vbNewLine & "You can press the abort button during the test run to stop the process. The test report will be incomplete", _
									  vbYesNo + vbExclamation, _
									  "Canvas Test Suite", _
									  "CanvasController")
										
		If (answer = vbNo) Then
			Exit Sub
		End If
	End If
	
	DesktopX.ScriptObject("CC_Select").Control.Enabled = False
	DesktopX.Object("CC_Button_OK_Text").Text = "Abort"
	DesktopX.ScriptObject("CC_Button_OK").SetCallBack GetRef("OnAbortTests")
	areTestRunning = True	
	
	' Show the test info (loading scripts)
	UpdateTestInfo -1

	CleanupResultsFolder()
	LoadTests()

	' Check that a least one test is present
	If (test_scripts.Count = 0) Then
		If (Not unattended) Then
			answer = DesktopX.MsgBox("No test could be loaded. Check that the generated tests scripts are present in the tests subfolder", _
									    vbOKOnly + vbCritical , _
									    "Canvas Test Suite", _
									    "CanvasController")
		End If		    
		ResetUI
		areTestRunning = False
		DesktopX.ScriptObject("CC_Select").Control.Enabled = True
		Exit Sub
	End If
	
	' Prepare report
	PrepareReport()

	' Start running tests
	current_test_index = 0	
	Object.KillTimer 2
	Object.SetTimer 2, 100
End Sub

' Load all the tests into the dictionnary
Sub LoadTests()
	Dim fso, folder, file, index

	' Reset the list of test scripts
	Set test_scripts = Nothing
	Set test_scripts = CreateObject("Scripting.Dictionary")

	' Load all js files from tests
	Set fso = CreateObject("Scripting.FileSystemObject")
	Set folder = fso.GetFolder(Widget.Preference("TestsFolder").Value & "/tests")
	
	index = 0
	For Each file In folder.Files
		If (LCase(Right(file.name, 2)) = "js") Then		
		  	test_scripts.Add index, file	
		  	index = index + 1		
		End If
	Next
End Sub

Sub PrepareReport()
	Dim root, info, tests, total, status, fail, pass, unknown, id, report
	
	' Reset counters
	tests_failed = 0
	tests_passed = 0
	tests_unknown = 0

	' Open report file
	Set test_report = CreateObject("Microsoft.XMLDOM") 
	test_report.async = False 
	test_report.preserveWhiteSpace = True	
	
	If test_report.Load(Widget.Preference("TestsFolder").Value & "/results.xml") Then
		' The document loaded successfully.
		Set root = test_report.selectSingleNode("/reports")
	Else
		' The document failed to load.
		' Add root node	if it doesn't exit yet
		Set root = test_report.createElement("reports")
		test_report.appendChild root
	End If


	'====================================================
	Set report = test_report.createElement("report")
	root.appendChild report
	
	Set id = test_report.createAttribute("id")	
	report.setAttribute "id", test_id
	
	'====================================================
	' Add tests info
	Set info = test_report.createElement("info")
	report.appendChild info
	
	Set total = test_report.createElement("total")
	total.Text = test_scripts.Count
	info.appendChild total
	
	Set status = test_report.createElement("status")
	status.Text = "OK"
	info.appendChild status
	
	Set fail = test_report.createElement("fail")
	fail.Text = "0"
	info.appendChild fail
	
	Set pass = test_report.createElement("pass")
	pass.Text = "0"
	info.appendChild pass
	
	Set unknown = test_report.createElement("unknown")
	unknown.Text = "0"
	info.appendChild unknown
	
	'====================================================
	' Add tests node
	Set tests = test_report.createElement("tests")
	report.appendChild tests
End Sub

' Callback to update the report with a single test result
Sub UpdateReport(name, description, expected, image, results, status)
	Dim nodes, tests, test
	Dim test_name, test_description, test_expected, test_image, test_results, test_status
	
	' Select the tests node
	Set tests = test_report.selectSingleNode("/reports/report[@id=""" & test_id & """]/tests")	
	
	Set test = test_report.createElement("test")
	tests.appendChild test
	
	Set test_name = test_report.createElement("name")
	test_name.Text = name
	test.appendChild test_name
	
	Set test_description = test_report.createElement("description")
	test_description.Text = description
	test.appendChild test_description
	
	Set test_expected = test_report.createElement("expected")
	test_expected.Text = expected
	test.appendChild test_expected
	
	Set test_image = test_report.createElement("image")
	test_image.Text = image
	test.appendChild test_image
	
	Set test_results = test_report.createElement("results")
	test_results.Text = results
	test.appendChild test_results
	
	Set test_status = test_report.createElement("status")	
	
	Select Case status
		Case STATUS_FAIL     
      		test_status.Text = "FAIL"
			tests_failed = tests_failed + 1
		Case STATUS_SUCCESS  
      		test_status.Text = "PASS"
			tests_passed = tests_passed + 1
		Case STATUS_MANUAL  
     		test_status.Text = "MANUAL"
			tests_unknown = tests_unknown + 1
		Case Else
			'Treat all other values as fail
      		test_status.Text = "FAIL"
			tests_failed = tests_failed + 1
	End Select
	
	test.appendChild test_status

	Object.SetTimer 2, 25
End Sub

Sub CloseReport(isAbort)
	
	If test_report Is Nothing Then	
		Exit Sub
	End If
	
	' Update info node with global status if aborted
	
	If (isAbort) Then
		SetNodeValue "/reports/report[@id=""" & test_id & """]/info/status", "ABORT"
	End If
	
	' Update tests values
	SetNodeValue "/reports/report[@id=""" & test_id & """]/info/pass", tests_passed
	SetNodeValue "/reports/report[@id=""" & test_id & """]/info/fail", tests_failed
	SetNodeValue "/reports/report[@id=""" & test_id & """]/info/unknown", tests_unknown
	
	' Save report file
	test_report.save(Widget.Preference("TestsFolder").Value & "/results.xml")
	Set test_report = Nothing	
End Sub

Sub SetNodeValue(path, value)
	Dim node
	Set node = test_report.selectSingleNode(path)
	node.Text = value
End Sub

Sub ShowReport

End Sub

Sub UpdateTestInfo(index)
	
	DesktopX.Object("CC_Tests_Info").Visible = True
	
	If (index = -1) Then
		DesktopX.Object("CC_Tests_Info").Text = "Preparing tests..."
		DesktopX.Object("CC_Tests_Info").ToolTipText = ""
		Exit Sub
	End If
	
	DesktopX.Object("CC_Tests_Info").Text = "Test " & index & "/" & test_scripts.Count
	DesktopX.Object("CC_Tests_Info").ToolTipText = "PASS: " & tests_passed & " - UNKNOWN: " & tests_unknown & " - FAIL: " & tests_failed  
End Sub

Sub CleanupResultsFolder
	Dim fso, file, results
	results = Widget.Preference("TestsFolder").Value & "/results" 
	Set fso = CreateObject("Scripting.FileSystemObject")
	
	' Check that the results folder exists
	If Not fso.FolderExists(results) Then
		fso.CreateFolder(results)
	End If
	
	' Create a new results folder
	For Each file In fso.GetFolder(results).Files
		file.delete
	Next
	
	Set fso = Nothing
End Sub

' Check that the test suite is presents (with at least one file in the tests folder)
Function CheckTestSuitePresent()
	Dim root, fso, testsFolder
	CheckTestSuitePresent = False
	root = Widget.Preference("TestsFolder").Value 
	
	' Make sure we have a proper value
	If root = "" Then
		Exit Function
	End If

	Set fso = CreateObject("Scripting.FileSystemObject")
		
	' Check that the root folder exists
	If Not fso.FolderExists(root) Then
		Exit Function
	End If
			
	' Check that the test folders are present
	If (Not fso.FolderExists(root & "/images")) Or _
	   (Not fso.FolderExists(root & "/output")) Or _
	   (Not fso.FolderExists(root & "/tests")) Or _
	   (Not fso.FileExists(root & "/include.js")) Then
	   Exit Function
	End If
	   
	' Check that there is a least one file in the tests folder
	Set testsFolder = fso.GetFolder(root & "/tests")
	If (testsFolder.Files.Count = 0) Then
		Exit Function
	End If
	
	Set fso = Nothing
	CheckTestSuitePresent = True
End Function

' Hide/Show tests icon
Function ShowTestsIcon(show)	
	DesktopX.Object("CC_Icons_Tests").Visible = show
End Function


'------------------------------------------------------------
'-- Scripts
'------------------------------------------------------------
Sub LoadScripts()
	If (areTestRunning) Then
		Exit Sub
	End If
	
	Object.KillTimer 1
	Object.SetTimer 1, 500
End Sub


Sub LoadScriptsInternal()
	Dim fso, folder, subfolder

	Set scripts = CreateObject("Scripting.Dictionary")
	
	DesktopX.ScriptObject("CC_Select").Control.ResetList()
	DesktopX.ScriptObject("CC_Select").Control.Enabled = False
	
	Set fso = CreateObject("Scripting.FileSystemObject")
		
	If Not fso.FolderExists(Widget.Preference("ScriptFolder").Value) Then
		DesktopX.Object("CC_Invalid").Visible = True
		Exit Sub
	Else
		DesktopX.Object("CC_Invalid").Visible = False
	End If
		
	
	' Load in root folder
	Set folder = fso.getFolder(Widget.Preference("ScriptFolder").Value)
	LoadScripFiles folder, ""
	
	' Subfolders
	For Each subfolder In folder.SubFolders
		'Ignore folders starting with .
		If (Not(left(subfolder.name, 1) = ".") And AreScripFilesPresent(subfolder)) Then
			scripts.Add subfolder.name, ""
			DesktopX.ScriptObject("CC_Select").Control.AddItem subfolder.name

			LoadScripFiles subfolder, "   "
		End If
	Next

	If (DesktopX.ScriptObject("CC_Select").Control.ItemCount > 0) Then
		If Not areTestRunning Then
			DesktopX.ScriptObject("CC_Select").Control.Enabled = True
		End If
		SelectFirstScript
	Else
		DesktopX.ScriptObject("CC_Select").Control.Enabled = False
	End If
	
End Sub

' Check that there exists at least one script file
Function AreScripFilesPresent(folder)
	Dim file
	
	AreScripFilesPresent = False
	
	For Each file In folder.Files		
		' VBScript
		If (LCase(Right(file.name, 3)) = "vbs") Then		
	  		AreScripFilesPresent = True
	  		Exit Function		
		End If
	
		If (LCase(Right(file.name, 2)) = "js") Then		
	  		AreScripFilesPresent = True
	  		Exit Function		
		End If
	Next
End Function


Sub LoadScripFiles(folder, prefix)
	Dim file
	Dim name
	
	For Each file In folder.Files	
		name = ""
		
		' VBScript
		If (LCase(Right(file.name, 3)) = "vbs") Then		
		  	name = prefix & Left(file.name, Len(file.Name) - 4)	  	
		  	scripts.Add name, file			
		End If
	
		' JScript
		If (LCase(Right(file.name, 2)) = "js") Then		
		  	name = prefix & Left(file.name, Len(file.Name) - 3)	  	
		  	scripts.Add name, file			
		End If
	
		If (name <> "") Then
			DesktopX.ScriptObject("CC_Select").Control.AddItem name
		End If
	Next
End Sub

Sub SelectFirstScript
	Dim script
	
	script = DesktopX.ScriptObject("CC_Select").Control.Text
	
	If Not(scripts.Exists(script)) Then
		DesktopX.ScriptObject("CC_Select").Control.ListIndex = 0
		Exit Sub
	End If
	
	'Check if this is a folder
	If scripts.Item(script) = "" Then
		DesktopX.ScriptObject("CC_Select").Control.ListIndex = 1
	End If
End Sub

Sub SelectPreviousScript()
	If (areTestRunning) Then
		Exit Sub
	End If

	Dim index
	
	index = DesktopX.ScriptObject("CC_Select").Control.ListIndex
	
	If (index > 0) Then
		index = index - 1
	End If
	
	DesktopX.ScriptObject("CC_Select").Control.ListIndex = index
End Sub

Sub SelectNextScript()
	If (areTestRunning) Then
		Exit Sub
	End If
	
	Dim index
	
	index = DesktopX.ScriptObject("CC_Select").Control.ListIndex
	
	If (index < DesktopX.ScriptObject("CC_Select").Control.ItemCount - 1) Then
		index = index + 1
	End If
	
	DesktopX.ScriptObject("CC_Select").Control.ListIndex = index
End Sub


Sub OnChangeScript(item, name)
	' Do nothing for now
End Sub

Sub OnApplyScript()
	If (areTestRunning) Then
		Exit Sub
	End If

	Dim script
	Dim path
	
	If (DesktopX.ScriptObject("CC_Select").Control.ItemCount = 0) Then
		Exit Sub
	End If
	
	script = DesktopX.ScriptObject("CC_Select").Control.Text
	
	If Not(scripts.Exists(script)) Then
		MsgBox "Invalid Script"
		Exit Sub
	End If
	
	'Check if this is a folder
	If scripts.Item(script) = "" Then
		Exit Sub
	End If
	
	RemoveCurrentScript()
	LoadScript scripts.Item(script)	
	StartScript False
End Sub

Sub StartScript(unnattended)
	If (currentScriptPath = "") Then
		Exit Sub
	End If
	
	On Error Resume Next
	DesktopX.ScriptObject(GetCanvasName(currentScriptPath)).Start(unnattended)
	Err.Clear
End Sub

' Call Object_OnScriptExit
Sub RemoveCurrentScript()

	' always hide test object
	DesktopX.Object("CanvasObjectTestSuite").Visible = False

	If currentScriptPath = "" Then
		Exit Sub
	End If
	
	Dim name, other
	name = ""
	other = ""

	If (LCase(Right(currentScriptPath, 3)) = "vbs") Then		
		name = "CanvasObject"
		other = "CanvasObjectJS"
	End If
	
	If (LCase(Right(currentScriptPath, 2)) = "js") Then		
		name = "CanvasObjectJS"
		other = "CanvasObject"
	End If
	
	If name = "" Then
		MsgBox "Error 1: There was an unexpected error when setting the script!", vbExclamation + vbOKOnly, "Error loading script" 
		Exit Sub
	End If
	
	On Error Resume Next
	DesktopX.ScriptObject(name).Object_OnScriptExit()
	
	If (Err.Number = 438) Then
		MsgBox "Script: "& currentScriptPath & vbNewLine & vbNewLine & "You either do not have a 'Object_OnScriptExit' function defined in your script file, or there is a syntax error in that function", vbExclamation + vbOKOnly, "Object_OnScriptExit function not present / Syntax error" 
	ElseIf (Err.Number <> 0) Then
		MsgBox "Error # " & CStr(Err.Number) & vbNewLine & vbNewLine & Err.Description, vbExclamation + vbOKOnly, "Error running script"
	End If
	Err.Clear
	
	ResetObject name
	
	currentScriptPath = ""
	
	' Move the other CanvasObject to the current coordinates
	DesktopX.Object(other).Top = DesktopX.Object(name).Top
	DesktopX.Object(other).Left = DesktopX.Object(name).Left	
End Sub

Function GetCanvasName(path)
	GetCanvasName = ""
	
	If (LCase(Right(path, 3)) = "vbs") Then		
		GetCanvasName = "CanvasObject"
	End If
	
	If (LCase(Right(path, 2)) = "js") Then		
		GetCanvasName = "CanvasObjectJS"
	End If
End Function

Sub LoadScript(path)
	Dim name
	name = GetCanvasName(path)
	
	If name = "" Then
		MsgBox "Error2: There was an unexpected error when setting the script!", vbExclamation + vbOKOnly, "Error loading script" 
		Exit Sub
	End If
	
	'Set the script For the canvas object
	If (areTestRunning = False) Then
		DesktopX.Object(name).Visible = True
	End If
	DesktopX.Object(name).SetScript path
	
	On Error Resume Next
	DesktopX.ScriptObject(name).Object_OnScriptEnter()	
			
	If (Err.Number = 438) Then
		MsgBox "Script: "& path & vbNewLine & vbNewLine & "You either do not have a 'Object_OnScriptEnter' function defined in your script file, or there is a syntax error in that function", vbExclamation + vbOKOnly, "Load function not present / Syntax error" 
	ElseIf (Err.Number <> 0) Then		
		MsgBox "Error # " & Err.Number & vbNewLine & vbNewLine & Err.Description, vbExclamation + vbOKOnly, "Error loading and running script "
	Else
		'Set the current script as loaded
		currentScriptPath = path
	End If
	
	Err.Clear
End Sub

Sub ResetObject(name)
	DesktopX.Object(name).SetScript ""	
	DesktopX.Object(name).Visible = False
	
	DesktopX.ScriptObject(name).Canvas.Width = 300
	DesktopX.ScriptObject(name).Canvas.Height = 150
	DesktopX.ScriptObject(name).Canvas.DebugMode = False
End Sub

'------------------------------------------------------------
'-- Preferences
'------------------------------------------------------------
Sub RegisterPreferences
	Widget.AddPreference "ScriptFolder"
	Widget.AddPreference "TestsFolder"
	Widget.AddPreference "ResetPosition"
	Widget.AddPreference "LockPosition"
	
	Widget.Preference("ScriptFolder").Type = "Folder"
	Widget.Preference("ScriptFolder").Caption = "Script Folder"
	Widget.Preference("ScriptFolder").DefaultValue = DesktopX.ExecutableDirectory & "/Scripts"
	Widget.Preference("ScriptFolder").Description = "The folder from which the scripts are loaded"
	
	Widget.Preference("TestsFolder").Type = "Folder"
	Widget.Preference("TestsFolder").Caption = "Tests Folder"
	Widget.Preference("TestsFolder").DefaultValue = DesktopX.ExecutableDirectory & "/Scripts/TestSuite"
	Widget.Preference("TestsFolder").Description = "The folder from which the test scripts are loaded"
	
	Widget.Preference("LockPosition").Type = "CheckBox"
	Widget.Preference("LockPosition").Caption = "Lock the Canvas position"
	Widget.Preference("LockPosition").DefaultValue = "0"
	Widget.Preference("LockPosition").Description = "The Canvas Controller will be locked at his current position."	
	
	Widget.Preference("ResetPosition").Type = "CheckBox"
	Widget.Preference("ResetPosition").Caption = "Reset Canvas position"
	Widget.Preference("ResetPosition").DefaultValue = "0"	
	Widget.Preference("ResetPosition").Description = "The Canvas Controller and the canvas surface positions will be reset to their initial values."
End Sub

Sub Widget_OnPreferencesChange()
	LoadScripts()
	ResetObject "CanvasObject"
	ResetObject "CanvasObjectJS"
	
	If Widget.Preference("ResetPosition").Value = "1" Then
		DesktopX.Object("CanvasController").Move 120,66
		DesktopX.Object("CanvasObject").Move 350,40
		DesktopX.Object("CanvasObjectJS").Move 350,40
		
	
		Widget.Preference("ResetPosition").Value = "0"
	End If
	
	If Widget.Preference("LockPosition").Value = "1" Then
		pLeft = Object.Left
		pTop = Object.Top
		DesktopX.Object("CC_Icons_Pin").State = "Pinned"
		pinned = True
	Else
		pinned = False
	End If
	
	ShowTestsIcon CheckTestSuitePresent	
	If Not CheckTestSuitePresent Then
		MsgBox "The test suite could not be found. Make sure the folder selected contains the test suite data folders and generated tests folder"
	End If
	
	SelectFirstScript
End Sub
