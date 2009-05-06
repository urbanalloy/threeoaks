Option Explicit 

Sub Object_OnScriptEnter 
    canvas.width = 200 
    canvas.height = 200 

    DrawCalendar() 
End Sub 

Sub Object_OnScriptExit 

End Sub 

' Three Sections Month/Year, Weekdays, Calendar Days 
Sub DrawCalendar() 
    Dim ctx 
    Dim x1,y1    ' Month/Year x,y positions 
    Dim w1, h1    ' Month/Year width, height 
    Dim x2,y2    ' Weekday Headings x,y positions 
    Dim w2, h2    ' Weekday Headings width, height 
    Dim x3, y3    ' Calendar x,y positions    
    Dim w3, h3    ' Calendar Days 
    Dim c, r    ' Column, Row space 
    Dim strDate 
    Dim pattern 
    Dim i,j 
    Dim curMth 
    Dim FirstDayOfWeek 
    Dim thismonth,thisyear,thisMonthName,calFirstDay 
    Dim calDaysInMonth, calDay 
    x1=5 
    y1=5 
    w1=canvas.width-(x1*2) 
    h1=25 
    x2=5 
    y2=(y1*2)+h1 
    w2=25 
    h2=15 
    x3=5 
    y3=y1+y2+h2 
    w3=w2 
    h3=25 
    c=2 
    r=2 
    strDate=MonthName(Month(Now))&" "&Year(Now) 
    ' Draw Background 
    Set ctx = canvas.getContext("2d") 
    ctx.fillStyle = "black" 
    ctx.fillRect 0, 0, 200, 200 
    ' Draw Month/Year 
    ctx.fillStyle = "white" 
    ctx.fillRect x1, y1, w1, h1 
    ctx.font = "Sans Bold 12" 
    ctx.fillStyle = "black" 
    ctx.textBaseline = "top"
    
    ctx.fillText strDate, x1+15, y1 'arrWkDay(i), x, y         

    ' Draw Weekday Headings 
    For i=0 to 6 
        If i=0 Then 
            x2=5            ' x coord of first column 
            ctx.fillStyle = "gray" 
        ElseIf i>0 And i<6 Then 
            x2=5+(i*(w2+c)) 
            ctx.fillStyle = "white" 
        ElseIf i=6 Then 
            x2=5+(i*(w2+c)) 
            ctx.fillStyle = "gray"            
        End If 
        ctx.fillRect x2, y2, w2, h2 
        ' Weekday Headings 
        ctx.font = "Sans 8" 
        ctx.fillStyle = "black" 
        ctx.fillText WeekDayName((i+1), True,1), x2+2, y2 'arrWkDay(i), x, y         
    Next 
    
    ' Calendar Area 
    ' Create Transparent background relative to Weeday headings 
    ctx.clearRect x3, y3, 200-(x3*2), 200-(y3+y1) 
    ' Create Gradient Background 
    Set pattern = ctx.createLinearGradient(0, 0, 0, 200-(y3+y1)) 
    pattern.addColorStop 1, "blue" 
    pattern.addColorStop 0, "rgba(255,255,255,0.5)" 
    ctx.fillStyle = pattern 
    ctx.rect x3, y3, 200-(x3*2), 200-(y3+y1) 
    ctx.fill    

    ' Get first day of week from system settings 
    If Weekday(MonthName(8) & " 7, 2004") = 7 Then 
        FirstDayOfWeek = 1 
    Else 
        FirstDayOfWeek = 2 
    End If 
    
    ' Draw Days 
    curMth = Now() 
    curMth = Month(curMth) & " " & Year(curMth) 

    ' Determine the month and year that we are drawing from the variable we passed to the function 
    thismonth = Left(curMth, InStr(curMth," ") -1) 
    thisyear = Right(curMth, Len(curMth) - Len(thismonth) - 1) 
    
    ' Get the month name and write it to the text object for the month name 
    thisMonthName = MonthName(thismonth) 

    ' Check which day number in the week the first day of the month occurs on 
    calFirstDay = thisMonthName & " 1, " & thisYear 
    calFirstDay = CDate(calFirstDay) 
    calFirstDay = DatePart("w", calFirstDay, FirstDayOfWeek) 
    ' Check how many days there are in the current month 
    calDaysInMonth = DaysInAnyMonth(thisMonth, thisYear) 
    x3=5 
    calDay=0 
    For j = calFirstDay To calFirstDay + calDaysInMonth -1            
        If j = calFirstDay + Day(Now()) - 1 Then 
            ctx.fillStyle = "red"    ' Highlight today 
        ElseIf j=0 Or j=7 Or j=8 Or j=14 Or j=15 Or j=21 Or j=22 Or j=28 Or j=29 Or j=35 Or j=36 Or j=42 Or j=43 Then 
            ctx.fillStyle = "gray"            ' Highlight Weekends 
        Else 
            ctx.fillStyle = "white"        
        End If 
        ctx.fillRect x3 + ((w3+c) * ((j-1) Mod 7)), y3 + (Int((j-1)/7)*(h3+r)), w3, h3 
        ' Calculate Day 
        calDay=calDay+1 
        ctx.font = "Sans 10" 
        ctx.fillStyle = "black" 
        ctx.fillText calDay, x3 + ((w3+c) * ((j-1) Mod 7)), y3 + (Int((j-1)/7)*(h3+r))        
        ' Create Transparent square to give a border illusion for each day 
        '**Note: Gradient will not show up in this sequence 
        'ctx.clearRect x + ((w+c) * ((j-1) Mod 7))+bw, y2 + (Int((j-1)/7)*(h2+r))+bw, w-(bw*2), h2-(bw*2) 
    Next 
    
End Sub 

' Function to return the number of days in a month for the month and year provided 
Function DaysInAnyMonth(intMonth, IntYear) 
    If IntYear = 0 Then 
    IntYear = Year(Now()) 
  End If 
  Select Case (intMonth) 
  Case 9, 4, 6, 11 
    DaysInAnyMonth = 30 
  Case 2 ' February (includes leap year calculation) 
  If (IntYear Mod 4 = 0) And (IntYear Mod 100 <> 0) Or (IntYear Mod 400 = 0) Then 
    DaysInAnyMonth = 29 
  Else 
    DaysInAnyMonth = 28 
     End If 
    Case Else 
    DaysInAnyMonth = 31 
    End Select 
End Function
