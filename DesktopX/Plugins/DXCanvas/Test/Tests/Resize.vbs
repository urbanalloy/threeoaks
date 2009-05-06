Option Explicit

Dim context

Sub Object_OnScriptEnter
    'Save original width and height      
    canvas.width = 200
    canvas.height = 200

    FillRect
	Object.SetTimer 1, 2000
End Sub

Sub Object_OnScriptExit
    Object.KillTimer 1
End Sub

Sub FillRect
    Set context = Canvas.getContext("2d")
	context.fillRect 0, 0, 200, 200 
End Sub

Sub Object_OnTimer1

    canvas.width = 125
    FillRect
    Object.Sleep 500
    
    canvas.height = 125
    FillRect
    Object.Sleep 500
    
    canvas.width = 200
    canvas.height = 200
    FillRect
End Sub