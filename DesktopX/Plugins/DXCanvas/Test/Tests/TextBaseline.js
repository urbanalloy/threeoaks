function object_onScriptEnter() {
    canvas.width = 600;
    canvas.height = 300;

    draw();
}

function object_onScriptExit() {
}


function draw() {
    var ctx = canvas.getContext('2d');
    ctx.save();
    ctx.fillStyle = "white";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    ctx.lineWidth = 4
    ctx.strokeRect(0, 0, canvas.width, canvas.height);
    ctx.restore();

	var x = 10;
	var y = 150;

	ctx.lineWidth = 1;
	ctx.beginPath();
	ctx.moveTo(0, 150);
	ctx.lineTo(600, 150);
	ctx.closePath();
	ctx.stroke();

	ctx.font = "Times New Roman 32px";

	var text = "Baseline";
	var w = ctx.measureText(text).width;
	ctx.fillText(text, x, y);
	x += w + 10;

	text = "Top";
	ctx.textBaseline = "top";
	w = ctx.measureText(text).width;
	ctx.fillText(text, x, y);
	x += w + 10;

	text = "Bottom";
	ctx.textBaseline = "bottom";
	w = ctx.measureText(text).width;
	ctx.fillText(text, x, y);
	x += w + 10;

	text = "Middle";
	ctx.textBaseline = "middle";
	w = ctx.measureText(text).width;
	ctx.fillText(text, x, y);
	x += w + 10;

}


