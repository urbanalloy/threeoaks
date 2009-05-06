function object_onScriptEnter() {
    canvas.width = 600;
    canvas.height = 600;

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

	var lingrad = ctx.createLinearGradient(0,0,600,600);
	lingrad.addColorStop(0, '#00ABEB');
	lingrad.addColorStop(1.0, '#fff');

	var x = 10;
	var y = 30;

	ctx.font = "Times New Roman 32px";
	ctx.fillText("Normal Fill Text", x, y);

	y += 40;

	ctx.lineWidth = 2;
	ctx.strokeText("Normal Stroke Text", x, y);

	y += 40;

	ctx.fillStyle = lingrad;
	ctx.fillText("Gradient Fill Text", x, y);

	y += 40;

	ctx.strokeStyle = lingrad;
	ctx.strokeText("Gradient Stroke Text", x, y);

	ctx.textAlign = "end";

	x = 590;
	y += 40;

	ctx.fillStyle = 'black';
	ctx.fillText("Normal Fill Text", x, y);

	y += 40;

	ctx.strokeStyle = 'black';
	ctx.lineWidth = 2;
	ctx.strokeText("Normal Stroke Text", x, y);

	y += 40;

	ctx.fillStyle = lingrad;
	ctx.fillText("Gradient Fill Text", x, y);

	y += 40;

	ctx.strokeStyle = lingrad;
	ctx.strokeText("Gradient Stroke Text", x, y);

	y += 40;
	x = 300;

	ctx.textAlign = "center";

	ctx.fillStyle = 'black';
	ctx.fillText("Normal Fill Text", x, y);

	y += 40;

	ctx.strokeStyle = 'black';
	ctx.lineWidth = 2;
	ctx.strokeText("Normal Stroke Text", x, y);

	y += 40;

	ctx.fillStyle = lingrad;
	ctx.fillText("Gradient Fill Text", x, y);

	y += 40;

	ctx.strokeStyle = lingrad;
	ctx.strokeText("Gradient Stroke Text", x, y);

y += 40;
}


