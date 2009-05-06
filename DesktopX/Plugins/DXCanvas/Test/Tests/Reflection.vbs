Option Explicit

Sub Object_OnScriptEnter
    Dim ctx

    canvas.width = 200
    canvas.height = 300

	DrawSpheresNB
	DrawReflection
End Sub

Sub Object_OnScriptExit

End Sub

Sub DrawReflection()
    Dim ctx
    Set ctx = canvas.getContext("2d")
    
    ctx.DrawImageRegion canvas, 0, 100, 200, 100, 0, 200
End Sub

Sub DrawSpheresNB
    Dim context
	Dim pattern

	Set context = canvas.getContext("2d")
	
	context.beginPath
	Set pattern = context.createLinearGradient(0, 0, 0, 200)
	pattern.addColorStop 1, "black"
	pattern.addColorStop 0, "rgba(255,255,255,0.5)"
	context.fillStyle = pattern
	context.rect 0, 0, 200, 200
	context.fill
	
	context.beginPath 'Needed to clear the previous path
	Set pattern = context.createRadialGradient(115.2, 102.4, 25.6, 102.4, 102.4, 128.0)	
	pattern.addColorStop 0, "white"
	pattern.addColorStop 1, "black"
	context.fillStyle = pattern	
	context.arc 128.0, 128.0, 76.8, 0, 2 * 3.14, FALSE
	context.fill
End Sub
