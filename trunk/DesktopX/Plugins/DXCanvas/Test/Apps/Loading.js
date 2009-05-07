// Small
/*
var radius = 18;
var padding = 3;
var lineWidth = 4;
var color = "#626262";
*/

// Large
var radius = 35;
var padding = 3;
var lineWidth = 6;
var color = "#ffffff";


var overSample = true;

var numTics = 12;

var color1 = "#ee1111";
var color2 = "#1122ee";
var runTime = 1500; // ms
var overSampleRate = 2; // 2 or more

var w = radius * 2 + padding * 2 + lineWidth/2;
var h = w;
var numFrames = numTics;

var currentFrame;
var ctx;

// Animation Type
var filename = "Loading";
var isMac = true;

// If you want to save the results
var save = false;

function Object_OnScriptEnter() {
  ctx = canvas.getContext('2d');

  canvas.width = w;
  canvas.height = h;
  
  if (overSample)
    numFrames *= overSampleRate;

  currentFrame = 0;
  Object.SetTimer(1, runTime/numFrames);
}

function object_onScriptExit() {
  Object.KillTimer(1);
}

function Object_OnTimer1() {
  canvas.suspendDrawing();
  
  currentFrame++;
  if (currentFrame > numFrames) {
    save = false;
    currentFrame = 0;
  }

  if (isMac)
    DrawFrameMac(currentFrame);
  else
    DrawFrame(currentFrame);
 
  canvas.resumeDrawing();
  
  if (save)
    canvas.toImage('D:/' + filename + '-' + (currentFrame < 10 ? '0' : '') + currentFrame + '.png');
}

function DrawFrameMac(frameNum) {
  frameNum++;

  ctx.clearRect(0, 0, w, h);

  ctx.lineWidth = lineWidth;
  ctx.lineCap = "round";
  ctx.strokeStyle = color;

  var ticAngle = 2 * Math.PI / numTics;
  var alphaStep = 1 / (numTics - 2);
  if (overSample) {
      var overSampleStep = (frameNum+1) % overSampleRate;
      frameNum = Math.floor(frameNum / overSampleRate);
  }

  ctx.save();
  
  ctx.translate(w/2, h/2);
  ctx.rotate(-Math.PI / 2 + frameNum * ticAngle);

  for (var i = 0; i < numTics; i++ ) {
      ctx.save()

      ctx.beginPath();
      ctx.rotate(i * ticAngle);
      ctx.moveTo(0, radius / 2);
      ctx.lineTo(0, radius);
      // for linear change:
      //ctx.globalAlpha = (i+1) * alphaStep;
      if (overSample)
          ctx.globalAlpha = Math.log(((i+1)*alphaStep + overSampleStep * (alphaStep/overSampleRate)) * (Math.E - 1) + 1);
      else
          ctx.globalAlpha = Math.log(((i+1)*alphaStep) * (Math.E - 1) + 1);
          
      ctx.stroke();

      ctx.restore();
  }
  
  ctx.restore();
}

function DrawFrame(frameNum) {
  frameNum++;

  ctx.clearRect(0, 0, w, h);

  ctx.lineWidth = lineWidth;

  ctx.strokeStyle = color1;
  var nextColor = color2;
  if (frameNum > numFrames / 2) {
      frameNum = frameNum - numFrames / 2;
      ctx.strokeStyle = color2;
      nextColor = color1;
  }

  ctx.beginPath();
  var circleTop = 0 - Math.PI / 2;
  var startAngle = circleTop;
  var endAngle = startAngle + (Math.PI * 2) * (frameNum / (numFrames/2));
  ctx.arc(w/2, h/2, radius, startAngle, endAngle, false);
  ctx.stroke();

  if (endAngle != Math.PI * 1.5) {
      ctx.beginPath();
      ctx.strokeStyle = nextColor;
      ctx.arc(w/2, h/2, radius, endAngle, 0 - Math.PI / 2, false);
      ctx.stroke();
  }

  //ctx.globalAlpha = 0.5;
  //ctx.fillStyle = "#F52100"; // red
  //ctx.fill();
}