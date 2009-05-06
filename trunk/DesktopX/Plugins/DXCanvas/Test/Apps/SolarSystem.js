var earth;
var moon;
var sun;

function object_onScriptEnter() {
    canvas.width = 300;
    canvas.height = 300;
    
    var ctx = canvas.getContext('2d');
    earth = ctx.loadImage(Widget.Preference("ScriptFolder").Value + '/tests/images/earth.png');
    sun = ctx.loadImage(Widget.Preference("ScriptFolder").Value + '/tests/images/sun_back.jpg');
    moon = ctx.loadImage(Widget.Preference("ScriptFolder").Value + '/tests/images/moon.png');

    Object.SetTimer(1, 100);
}

function object_onScriptExit() {
    Object.KillTimer(1);
}

function Object_OnTimer1() {
    canvas.suspendDrawing();
    DrawSolarSystem();
    canvas.resumeDrawing();
}

function DrawSolarSystem() {
    var ctx = canvas.getContext('2d');

    ctx.globalCompositeOperation = 'destination-over';
    ctx.clearRect(0,0,300,300); // clear canvas

    ctx.fillStyle = 'rgba(0,0,0,0.4)';
    ctx.strokeStyle = 'rgba(0,153,255,0.4)';
    ctx.save();
    ctx.translate(150,150);

    // Earth
    var time = new Date();
    ctx.rotate( ((2*Math.PI)/60)*time.getSeconds() + ((2*Math.PI)/60000)*time.getMilliseconds() );
    ctx.translate(105,0);
    ctx.fillRect(0,-12,50,24); // Shadow
    ctx.drawImage(earth, -12, -12, 75, 75);

    // Moon
    ctx.save();
    ctx.rotate( ((2*Math.PI)/6)*time.getSeconds() + ((2*Math.PI)/6000)*time.getMilliseconds() );
    ctx.translate(0,28.5);
    ctx.drawImage(moon, -3.5, -3.5, 25, 25);
    ctx.restore();

    ctx.restore();

    ctx.beginPath();
    ctx.arc(150,150,105,0,Math.PI*2,false); // Earth orbit
    ctx.stroke();

    ctx.drawImage(sun,0,0,300,300);

}