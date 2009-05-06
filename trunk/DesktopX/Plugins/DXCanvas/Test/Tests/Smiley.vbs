Option Explicit

Sub Object_OnScriptEnter

    canvas.width = 200
    canvas.height = 200
    
	Dim context
	Dim pattern
	Dim PI
	PI = 3.14159265

	Set context = canvas.getContext("2d")
		
	context.beginPath
    context.arc 75, 75, 50, 0, PI*2, TRUE ' Outer circle
    context.moveTo 110, 75
    context.arc 75, 75, 35, 0, PI, FALSE  ' Mouth (clockwise)
    context.moveTo 65, 65
    context.arc 60, 65, 5, 0, PI*2, TRUE  ' Left eye
    context.moveTo 95, 65
    context.arc 90, 65, 5, 0, PI*2, TRUE  ' Right eye
    context.stroke

End Sub

Sub Object_OnScriptExit

End Sub