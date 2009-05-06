function object_onScriptEnter() {
    canvas.width = 350;
    canvas.height = 300;

    draw();
}

function object_onScriptExit() {
}


function draw() {
    var ctx = canvas.getContext('2d');

    ctx.fillStyle = "white";
	ctx.rect(0, 0, 350, 300);
	ctx.fill();

	ctx.translate(10, 25);

	//ctx.beginPath();
	var pattern = ctx.createLinearGradient(0, -25, 0,25);
	pattern.addColorStop(1, "rgba(0, 0, 0, 1)");
	pattern.addColorStop(0, "white");
	
	ctx.font = "Sans Bold 10";
	ctx.fillStyle = pattern;
	var size = ctx.measureText("\"This is\tDesktopX\"");
	ctx.fillText("\"This is\tDesktopX\" width is " + size.width + "px", 0, 0);

	ctx.translate(0, 35);

	ctx.save();

	ctx.font = "Sans Bold 25";
	ctx.strokeStyle = 'black';
	ctx.shadowBlur = 5;
	ctx.shadowOffsetX = 2;
	ctx.shadowOffsetY = 2;
	ctx.shadowColor = "red"

	ctx.strokeText("Hello DesktopX!", 0, 0);
	
	ctx.translate(0, 80);

	ctx.font = "Segoe UI 11";
	ctx.fillStyle = 'black';
	ctx.shadowOffsetX = 1;
	ctx.shadowOffsetY = 1;
	ctx.shadowColor = "lightgrey"	

	ctx.fillText("We stand in our own light wherever we go,", 0, 0);
	ctx.fillText("and fight our own shadows forever.", 0, 17);

	ctx.restore();

	ctx.translate(0, 80);
	
	ctx.font = "Segoe UI Italic 11";
	ctx.fillStyle = 'black';
	
	ctx.fillText("- Owen Meredith", 170, 35);
	

}


