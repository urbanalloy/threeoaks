Option Explicit

Sub Object_OnScriptEnter
    canvas.width = 150
    canvas.height = 150
    
    Draw
    
    'canvas.toImage(Widget.Preference("ScriptFolder").Value & "//Test.png")   
End Sub

Sub Object_OnScriptExit

End Sub

Sub Draw  
    Randomize 
    
    Dim ctx, j

    Set ctx = canvas.getContext("2d")
    
    ctx.translate 75,75

    DrawOuterBackground(ctx)
    DrawBackground(ctx)

    ' draw stars
    For j = 1 To 49
        ctx.save
        ctx.fillStyle = "#fff"
        ctx.translate 75-Int(Rnd*150), 75-Int(Rnd*150)
        drawStar ctx,Int(Rnd*4)+2
        ctx.restore
    Next
End Sub

Sub DrawOuterBackground(ctx)
    Dim pattern
    
    ctx.save
    
     ' Create a circular clipping path        
    ctx.beginPath 
    ctx.arc 0,0,67,0,4*Atn(1)*2, TRUE
    ctx.clip

    ' draw background
    Set pattern = ctx.createLinearGradient(0, -75, 0, 75)
	pattern.addColorStop 1, "rgba(0, 0, 0, 1)"
	pattern.addColorStop 0, "rgba(255, 255, 255, 0.7)"

    ctx.fillStyle = pattern
    ctx.fillRect -75,-75,150,150
    
    ctx.restore
End Sub

Sub DrawBackground(ctx)
    Dim lingrad
    
    ' Create a circular clipping path        
    ctx.beginPath 
    ctx.arc 0,0,60,0,4*Atn(1)*2, TRUE
    ctx.clip

    ' draw background
    Set lingrad = ctx.createLinearGradient(0,-75,0,75)
    lingrad.addColorStop 0, "#232256"
    lingrad.addColorStop 1, "#143778"

    ctx.fillStyle = lingrad
    ctx.fillRect -75,-75,150,150
End Sub

Sub drawStar(ctx,r)
    ctx.save
    ctx.beginPath
    ctx.moveTo r,0
    
    Dim i
    For i = 0 To 8
        ctx.rotate 4*Atn(1)/5
        
        If (i Mod 2 = 0) Then
            ctx.lineTo (r/0.525731)*0.200811,0
        Else
            ctx.lineTo r,0
        End If
    Next
    
    ctx.closePath
    ctx.fill
    ctx.restore
End Sub
