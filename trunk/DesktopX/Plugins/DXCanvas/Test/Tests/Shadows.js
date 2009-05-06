var ctx = canvas.getContext('2d');

function object_onScriptEnter() {
    canvas.width = 350;
    canvas.height = 300;

    ctx.fillStyle = "white";
    ctx.fillRect(0, 0, 350, 300);

    drawBox();
    drawPicture();
    drawText();
}

function object_onScriptExit() {

}

function drawBox() {
    ctx.save();
    
    ctx.shadowBlur = 5;
    ctx.shadowColor = "red";
    
    ctx.fillStyle = "grey";
    ctx.fillRect(25, 25, 100, 100);

    ctx.restore();
}

function drawPicture() {
    ctx.save();
    
    var rhino = ctx.loadImage(Widget.Preference("ScriptFolder").Value + '/tests/images/rhino.jpg');

    ctx.shadowOffsetX = 2;
    ctx.shadowOffsetY = 2;
    ctx.shadowColor = "blue";
    
    ctx.drawImage(rhino, 150, 25);

    ctx.restore();
}

function drawText() {
    ctx.save();
    
    ctx.translate(15, 200);

    ctx.fillStyle = 'black';
    ctx.font = "Segoe UI 11";
    ctx.shadowOffsetX = 1;
    ctx.shadowOffsetY = 1;
    ctx.shadowBlur = 2;
    ctx.shadowColor = "lightgrey"



    ctx.fillText("We stand in our own light wherever we go,", 0, 0);
    ctx.fillText("and fight our own shadows forever.", 0, 17);

    ctx.font = "Segoe UI Italic 11";

    ctx.shadowOffsetX = 0;
    ctx.shadowOffsetY = 0;
    ctx.shadowBlur = 0;
    ctx.shadowColor = "transparent";

    ctx.fillText("- Owen Meredith", 170, 35);

    ctx.restore();
}

