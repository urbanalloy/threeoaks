var text = "It was a dream... Oh Just a dream...";

function object_onScriptEnter() {
    canvas.width = 800;
    canvas.height = 600;

    draw();
}


function object_onScriptExit() {
}

function draw()
{
  var ctx = canvas.getContext('2d');
  
  ctx.fillStyle = 'white';
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  x = 150;
  y = 550;
  ctx.moveTo (x, y);
  ctx.bezierCurveTo(x + 250, y + 50, x + 600, y - 50, x + 600, y - 250);
  x = x + 600;
  y = y - 250;
  ctx.bezierCurveTo (x+0, y-400, x-300, y-100, x-800, y-300);

  ctx.lineWidth = 1.5;
  ctx.strokeStyle = "red";
  ctx.stroke();
  
  ctx.font = "Serif 75";
  ctx.fillStyle = 'lightskyblue';
  ctx.strokeStyle = "steelblue";

  ctx.textAlongPath(text, false);
  ctx.textAlongPath(text, true);  
}