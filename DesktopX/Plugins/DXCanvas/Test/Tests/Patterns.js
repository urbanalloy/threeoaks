var earth;

function object_onScriptEnter() {
    canvas.width = 500;
    canvas.height = 500;

    var ctx = canvas.getContext('2d');
    earth = ctx.loadImage(Widget.Preference("ScriptFolder").Value + '/tests/images/sun.png');

    drawCircle();
}

function object_onScriptExit() {
}


function drawCircle() {
    var ctx = canvas.getContext('2d');

    ctx.clearRect(0, 0, 300, 300); // clear canvas

    var pattern = ctx.createPattern(earth, "repeat");
    ctx.lineWidth = 5;
    ctx.fillStyle = pattern;

    ctx.beginPath();
    ctx.arc(150, 150, 300, 0, Math.PI * 2, false); // Earth orbit
    ctx.fill();
}