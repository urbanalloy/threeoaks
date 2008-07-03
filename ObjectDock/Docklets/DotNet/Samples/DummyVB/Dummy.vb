Imports System
Imports System.Runtime.InteropServices
Imports System.Runtime.InteropServices.ComTypes
Imports System.Drawing
Imports System.Windows.Forms

Imports ObjectDockSDK
Imports ObjectDockSDK.Utils

Namespace DummyVB

    <Guid("81DE71EC-1635-4c7f-8675-EFDA8A5DCBE4")> _
    <ComVisible(True)> _
    <ClassInterface(ClassInterfaceType.None)> _
    Public Class DummyVB : Implements IDockletInterface

        Private Docklet As Docklet

        Public Sub OnGetInformation(ByRef name As String, ByRef author As String, ByRef version As Integer, ByRef notes As String) Implements IDockletInterface.OnGetInformation
            AssemblyData.GetInformation(name, author, version, notes)
        End Sub

        Public Sub OnCreate(ByVal data As DOCKLET_STATIC_DATA, ByVal ini As String, ByVal iniGroup As String) Implements IDockletInterface.OnCreate

            ODConsole.Initialize()

            Docklet = New Docklet(data)

            Docklet.Label = "DummyVB"
            Docklet.ImageFile = "DummyVB.png"

            Dim ContextMenu As Menu = New ContextMenu()

            Dim config As MenuItem = New MenuItem("Configure", AddressOf onMenuConfigure)

            Dim testmenu As MenuItem = New MenuItem("Test Menu")
            testmenu.MenuItems.Add("Item 1")
            testmenu.MenuItems.Add("Item 2")

            ContextMenu.MenuItems.Add(config)
            ContextMenu.MenuItems.Add(testmenu)

            Docklet.ContextMenu = ContextMenu
        End Sub

        Private Sub ShowInfo()
            'Label
            Dim sLabel As String = Docklet.Label
            'Visibility
            Dim sVisible As String = Docklet.IsVisible.ToString
            'Rectangle
            Dim sRect As String = Docklet.Rect.ToString
            'Edge & Align
            Dim sEdge As String = Docklet.DockEdge.ToString
            Dim sAlign As String = Docklet.DockAlign.ToString

            MsgBox(sLabel & vbCrLf & sVisible & vbCrLf & sRect & vbCrLf & sEdge & " - " & sAlign, MsgBoxStyle.Information)

        End Sub

        Public Sub OnSave(ByVal ini As String, ByVal iniGroup As String, ByVal isForExport As Boolean) Implements IDockletInterface.OnSave
            '
        End Sub

        Public Sub OnDestroy() Implements IDockletInterface.OnDestroy
            '
        End Sub

        Public Function OnExportFiles() As String() Implements IDockletInterface.OnExportFiles
            Dim oArray(2) As String

            oArray(0) = "DummyVB.png"
            oArray(1) = "DummyVB.dll"
            oArray(2) = "docklet.ini"

            Return oArray
        End Function

        Private Sub onMenuConfigure(ByVal sender As Object, ByVal e As EventArgs)
            Call OnConfigure()
        End Sub

        Public Function OnConfigure() As Boolean Implements IDockletInterface.OnConfigure
            Docklet.DoAttentionAnimation()
            Call ShowInfo()
            Docklet.DefaultConfigDialog()

            Return True
        End Function

        Public Function OnLeftButtonClick(ByRef ptCursor As Point, ByRef sizeDocklet As Size) As Boolean Implements IDockletInterface.OnLeftButtonClick
            Call OnConfigure()
            Return True
        End Function

        Public Function OnDoubleClick(ByRef ptCursor As Point, ByRef sizeDocklet As Size) As Boolean Implements IDockletInterface.OnDoubleClick
            Return False
        End Function

        Public Function OnLeftButtonHeld(ByRef ptCursor As Point, ByRef sizeDocklet As Size) As Boolean Implements IDockletInterface.OnLeftButtonHeld
            Return False
        End Function

        Public Function OnRightButtonClick(ByRef ptCursor As Point, ByRef sizeDocklet As Size) As Boolean Implements IDockletInterface.OnRightButtonClick
            Docklet.ShowContextMenu(ptCursor)
            Return True
        End Function

        Public Function OnAcceptDropFiles() As Boolean Implements IDockletInterface.OnAcceptDropFiles
            MsgBox("Accept Drop files...", MsgBoxStyle.Information)
            Return True
        End Function

        Public Sub OnDropFiles(ByVal hDrop As IntPtr) Implements IDockletInterface.OnDropFiles
            Dim sStr, sFiles As String
            Dim oArray() As String

            sStr = "Dropped file..." & vbCrLf & Docklet.GetCoordinates(hDrop).ToString() & vbCrLf

            oArray = Docklet.GetFiles(hDrop)

            sStr &= oArray.Length & vbCrLf

            For Each sFiles In oArray
                sStr &= sFiles & vbCrLf
            Next

            MsgBox(sStr, MsgBoxStyle.Information)

        End Sub

        Public Sub OnProcessMessage(ByVal hwnd As IntPtr, ByVal uMsg As UInteger, ByVal wParam As IntPtr, ByVal lParam As IntPtr) Implements IDockletInterface.OnProcessMessage

        End Sub

    End Class

End Namespace