Option Explicit

Sub Object_OnScriptEnter

    canvas.width = 200
    canvas.height = 200

	Dim context
	Dim pattern

	Set context = canvas.getContext("2d")
		
	context.fillRect 25, 25, 100, 100
	context.clearRect 45, 45, 60, 60
	context.strokeRect 50, 50, 50, 50
End Sub

Sub Object_OnScriptExit

End Sub

