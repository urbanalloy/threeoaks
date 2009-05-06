var ctx;
var pass_x = 0;
var last_x = 0;
var last_y = 0;
var spacing = 20;
var max_thinkness = 10;
var stop = false;
var anim1;

var canvas_width = 800;
var canvas_height = 500;

var red;
var green;
var blue;

function object_onScriptEnter() {
    canvas.width = canvas_width;
    canvas.height = canvas_height;

    init();
}

function object_onScriptExit() {
    Object.KillTimer(1);
}

function Object_OnTimer1() {
    canvas.suspendDrawing();
    randomPixel(ctx, canvas_width, canvas_height);
    canvas.resumeDrawing();
}

function init() {
    ctx = canvas.getContext('2d');
    ctx.beginPath();
    ctx.moveTo(0, 0);
    randomizeColor();

    Object.SetTimer(1, 50);
}

function randomPixel(ctx, width, height) {
	//set color
	green = shiftColor(green);
	red = shiftColor(red);
	blue = shiftColor(blue);

	if(pass_x >= width)
		pass_x = 0;
	
	pass_x += spacing;
	last_x = pass_x;

	var color = "rgb("+red+","+green+","+blue+")";

	var thinkness = Math.floor(Math.random() * max_thinkness);

	for(i = 0; i <= (height/spacing); i++) {
		var randomX = Math.floor(Math.random() * 3) - 1;
		
		if(last_y >= height) {
			last_y = 0;
			ctx.moveTo(0, 0);
		} else {
			last_y += spacing;
		}
		
		if(last_x >= width) {
			last_x = 0;
		} else if(last_x < 0) {
			last_x = width;
		} else {
			last_x += randomX*spacing;
		}
		
		putPixel(ctx, last_x, last_y, color, thinkness);
	}
}

function clearCanvas() {
	ctx.clearRect(0,0,canvas_width,canvas_height);
}

function shiftAllColors() {
	red += 127;
	green += 127;
	blue += 127;
}

function shiftColor(color) {
	var shift = Math.floor(Math.random() * 3) * 21;
	color += (Math.floor(Math.random() * 3) - 1)*shift;

	if(color >= 255) {
		color -= 30;
	} else if(color <= 0) {
		color += 30;
	}
	return color;
}

function putPixel(ctx,x,y,color, thinkness){
	clearPixel(ctx, x, y);

	//adjust spacing to thinkness
	x = x + (spacing/2 - thinkness/2);
	y = y + (spacing/2 - thinkness/2);
  	ctx.fillStyle = color;
  	ctx.fillRect(x,y,thinkness,thinkness);
}

function clearPixel(ctx,x,y){
  	ctx.clearRect(x,y,spacing,spacing);
}

function randomizeColor() {
	red = Math.floor(Math.random() * 255);
	green = Math.floor(Math.random() * 255);
	blue = Math.floor(Math.random() * 255);
}

