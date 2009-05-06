Option Explicit

Sub Object_OnScriptEnter
    Dim ctx

    canvas.width = 400
    canvas.height = 200

    DrawSoccer
    
    Set ctx = canvas.getContext("2d")
    ctx.translate 200, 0
	DrawSpheresNB
End Sub

Sub Object_OnScriptExit

End Sub

Sub DrawSoccer()
    Dim ctx
    Dim lingrad
    Dim lingrad2
    Set ctx = canvas.getContext("2d")
    
    ctx.beginPath

    ' Create gradients
    Set lingrad = ctx.createLinearGradient(0,0,0,150)
    lingrad.addColorStop 0, "#00ABEB"
    lingrad.addColorStop 0.5, "#fff"
    lingrad.addColorStop 0.5, "#26C000"
    lingrad.addColorStop 1, "#ffffff"

    Set lingrad2 = ctx.createLinearGradient(0,50,0,95)
    lingrad2.addColorStop 0.5, "#000"
    lingrad2.addColorStop 1, "rgba(0,0,0,0)"

    ' assign gradients to fill and stroke styles
    ctx.fillStyle = lingrad
    ctx.strokeStyle = lingrad2

    ' draw shapes
    ctx.fillRect 10,10,130,130
    ctx.strokeRect 50,50,50,50
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
