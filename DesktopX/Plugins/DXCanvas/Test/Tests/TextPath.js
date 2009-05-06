var manifesto = 
'The Internet is becoming an increasingly important part of our lives. ' + 
'The Mozilla project is a global community of people who believe that openness, innovation, and opportunity are key to the continued health of the Internet. We have worked together since 1998 to ensure that the Internet is developed in a way that benefits everyone. We are best known for creating the Mozilla Firefox web browser. ' + 
'The Mozilla project uses a community-based approach to create world-class open source software and to develop new types of collaborative activities. We create communities of people involved in making the Internet experience better for all of us. ' +
'As a result of these efforts, we have distilled a set of principles that we believe are critical for the Internet to continue to benefit the public good as well as commercial aspects of life';

function object_onScriptEnter() {
    canvas.width = 600;
    canvas.height = 200;

    draw();
}

function object_onScriptExit() {
}

function draw()
{
  var width = canvas.width;
  var height = canvas.height;
  var ctx = canvas.getContext('2d');
  ctx.fillStyle = 'white';
  ctx.fillRect(0, 0, width, height);
  ctx.save();
  ctx.translate(40, height/2-40);
  ctx.font = "Sans Bold 50";
  ctx.pathText("D X C a n v a s");
  ctx.strokeStyle = 'red';
  ctx.stroke();
  ctx.fillStyle = 'black';
  ctx.fill();
  ctx.fillStyle = 'grey';
  ctx.font = "Sans Bold 6";

//  ctx.shadowBlur = 5;
//  ctx.shadowColor = "red";
  
  ctx.textAlongPath(manifesto, false);
  ctx.restore();
}
