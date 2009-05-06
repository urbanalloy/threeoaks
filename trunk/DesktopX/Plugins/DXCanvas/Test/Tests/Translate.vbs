Option Explicit

Sub Object_OnScriptEnter

    canvas.width = 400
    canvas.height = 400
    
    DrawTranslate
  
End Sub

Sub Object_OnScriptExit

End Sub

Sub DrawTranslate()
    Dim ctx
    Dim i
    Dim j
    
    Set ctx = canvas.getContext("2d")
    
    ctx.fillRect 0,0,300,300
    
    For i = 0 To 2
        For j = 0 To 2
            ctx.save
            ctx.strokeStyle = "#9CFF00"
            ctx.translate 50+j*100, 50+i*100
            drawSpirograph ctx, 20*(j+2)/(j+1), -8*(i+3)/(i+1), 1
            ctx.restore
        Next
    Next

End Sub

Sub drawSpirograph(ctx, R, r0, O)
    Dim x1, y1, x2, y2, i

    x1 = R-O
    y1 = 0
    i  = 1
    
    ctx.beginPath
    ctx.moveTo x1, y1
    
    Do 
        If ( i > 20000) Then
             Exit Do
        End If
        
        x2 = (R+r0)*Cos(i*4*Atn(1)/72) - (r0+O)*Cos(((R+r0)/r0)*(i*4*Atn(1)/72))
        y2 = (R+r0)*Sin(i*4*Atn(1)/72) - (r0+O)*Sin(((R+r0)/r0)*(i*4*Atn(1)/72))
        
        ctx.lineTo x2, y2
        
        x1 = x2
        y1 = y2
        i = i + 1
    Loop While (x2 <> R-O And y2 <> 0)
    
    ctx.stroke
    
End Sub    
