Option Explicit

Sub Object_OnScriptEnter
    canvas.width = 200
    canvas.height = 200
    
    Clock
    Object.SetTimer 1, 1000    
End Sub

Sub Object_OnScriptExit
    Object.KillTimer 1
End Sub

Sub Object_OnTimer1
    canvas.suspendDrawing
    Clock
    canvas.resumeDrawing
End Sub

Sub Clock
    Dim now, ctx, PI, i
    Dim sec, min, hr
    PI = 4*Atn(1)
    
    Set ctx = canvas.getContext("2d")
    
    ctx.save
    ctx.clearRect 0,0,150,150
    ctx.translate 75,75
    ctx.scale 0.4,0.4
    ctx.rotate -PI/2
    ctx.strokeStyle = "black"
    ctx.fillStyle = "white"
    ctx.lineWidth = 8
    ctx.lineCap = "round"

    ' Hour marks
    ctx.save
    For i = 0 To 11
        ctx.beginPath
        ctx.rotate PI/6
        ctx.moveTo 100,0
        ctx.lineTo 120,0
        ctx.stroke
    Next
    ctx.restore

    ' Minute marks
    ctx.save
    ctx.lineWidth = 5
    For i = 0 To 59
        If i Mod 5 <> 0 Then
            ctx.beginPath
            ctx.moveTo 117,0
            ctx.lineTo 120,0
            ctx.stroke
        End If
        ctx.rotate PI/30
    Next
    ctx.restore

    sec = Second(Time)
    min = Minute(Time)
    hr  = Hour(Time)
    If hr >= 12 Then
        hr = hr - 12
    End If
   
    ctx.fillStyle = "black"

    ' write Hours
    ctx.save
    ctx.rotate  hr*(PI/6) + (PI/360)*min + (PI/21600)*sec
    ctx.lineWidth = 14
    ctx.beginPath
    ctx.moveTo -20,0
    ctx.lineTo 80,0
    ctx.stroke 
    ctx.restore

    ' write Minutes
    ctx.save
    ctx.rotate (PI/30)*min + (PI/1800)*sec
    ctx.lineWidth = 10
    ctx.beginPath
    ctx.moveTo -28,0
    ctx.lineTo 112,0
    ctx.stroke
    ctx.restore

    ' Write seconds
    ctx.save
    ctx.rotate sec * PI/30
    ctx.strokeStyle = "#D40000"
    ctx.fillStyle = "#D40000"
    ctx.lineWidth = 6
    ctx.beginPath
    ctx.moveTo -30,0
    ctx.lineTo 83,0
    ctx.stroke
    ctx.beginPath
    ctx.arc 0,0,10,0,PI*2,TRUE
    ctx.fill
    ctx.beginPath
    ctx.arc 95,0,10,0,PI*2,TRUE
    ctx.stroke
    ctx.fillStyle = "#555"
    ctx.arc 0,0,3,0,PI*2,TRUE
    ctx.fill
    ctx.restore

    ctx.beginPath
    ctx.lineWidth = 14
    ctx.strokeStyle = "#325FA2"
    ctx.arc 0,0,142,0,PI*2,TRUE
    ctx.stroke

    ctx.restore
    
    'canvas.toImage "D:\\Clock-" & hr & "-" & min & "-" & sec & ".png"
End Sub