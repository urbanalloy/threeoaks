function object_onScriptEnter() {
    canvas.width = 400;
    canvas.height = 150;

    draw();

    var ctx = canvas.getContext('2d');
    ctx.translate(200, 0);
    drawFrame();
}

function object_onScriptExit() {
}

function draw() {
    var ctx = canvas.getContext('2d');
    var img = ctx.loadImage(Widget.Preference("ScriptFolder").Value + '/tests/images/rhino.jpg');
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            ctx.drawImage(img, j * 50, i * 38, 50, 38);
        }
    }
}

function drawFrame() {
    var ctx = canvas.getContext('2d');
    var rhino = ctx.loadImage(Widget.Preference("ScriptFolder").Value + '/tests/images/rhino.jpg');
    var frame = ctx.loadImage(Widget.Preference("ScriptFolder").Value + '/tests/images/frame.png');

    // Draw slice
    ctx.drawImageRegion(rhino, 18, 33, 53, 61, 21, 20, 87, 104);

    // Draw frame
    ctx.drawImage(frame, 0, 0);
}


