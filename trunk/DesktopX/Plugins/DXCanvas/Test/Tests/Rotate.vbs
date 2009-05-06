Option Explicit

Sub Object_OnScriptEnter
	canvas.width = 200
	canvas.height = 200

	DrawRotate
End Sub

Sub Object_OnScriptExit

End Sub

Sub DrawRotate()
    Dim ctx, i, j
    Set ctx = canvas.getContext("2d")
    
    ctx.translate 100, 100
    
    For i = 1 To 5 ' Loop through rings (from inside to out)
        ctx.save
        ctx.fillStyle = "rgb(" & (51*i) & "," & (255-51*i) & ",255)"

        For j = 0 To i*6 'draw individual dots
            ctx.rotate 4*Atn(1)*2/(i*6)
            ctx.beginPath
            ctx.arc 0, i*12.5, 5, 0, 4*Atn(1)*2, TRUE
            ctx.fill
        Next
        ctx.restore
    Next

    
End Sub