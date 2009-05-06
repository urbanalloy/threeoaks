var ctx;

function object_onScriptEnter() {
    canvas.width = 300;
    canvas.height = 300;

    init();
}

function object_onScriptExit() {
    Object.KillTimer(1);
}

function Object_OnTimer1() {
    canvas.suspendDrawing();
    draw();
    canvas.resumeDrawing();
}


function init() {
    ctx = canvas.getContext('2d');    

    Object.SetTimer(1, 40);
}
 
 
function c()
{
   return Math.floor(Math.random()*256);}
 
 function c2()
 {
   return c()+','+c()+','+c()+','+Math.random();
}
 
 function p()
 {
   this.x = 150;
   this.y = 290;
   this.bounce = 0.99;       
   this.radius = (Math.random()*20)+2;
   this.fillStyle = 'rgba('+ c2() +')';
   this.energy();
}
 
 p.prototype.energy = function()
 {
   this.vy = (Math.random()*15)-40;
   this.vx = (Math.random()*20)-10;
   this.life = ( Math.random() * 100 ) + 100;
}
 
 p.prototype.move = function()
 {
   this.life--;
   this.y += this.vy;
   this.x += this.vx;

   if( (this.y+this.radius) > 300 )
   {
     this.y = 300-this.radius;
     this.vy = -this.vy*this.bounce;
   }
   
   if( (this.x-this.radius) < 0 )
   {
     this.x = this.radius;
     this.vx = -this.vx*this.bounce;
   }
   if( this.x+this.radius > 300 )
   {
     this.x = 300 - this.radius;
     this.vx = -this.vx*this.bounce;
   }

   this.vx *= gDrag;
   this.vy *= gDrag;
   this.vy += gGravity;
}
 
 p.prototype.draw = function( ctx )
 {
   ctx.beginPath();
   ctx.fillStyle = this.fillStyle;
   ctx.arc(this.x,this.y,this.radius,0,Math.PI*2,true); // Outer circle
   ctx.fill();

   ctx.strokeStyle = 'rgba(0,0,0,0.5)';
   ctx.stroke();
 }

 function draw()
{

 ctx.fillStyle = "#FFFFFF";
 ctx.fillRect(0, 0, 300, 300);
 ctx.lineWidth = 1.0;
    for( i=0; i<gParticles.length; i++ )
   {
     if( gParticles[i].life < 0 )
       gParticles[i].energy();
     else
     {
       gParticles[i].move();
       gParticles[i].draw( ctx ); 
     }
   }
 }
 
 var gGravity = 2;
 var gDrag = 0.98;
 var gFloor = 0.96;
 var gAnimate = true;
 var gParticles = new Array();
 for( i=0; i<15; i++)
   gParticles.push( new p() );


 function mm_new()
 {
   gParticles = new Array();
   for( i=0; i<15; i++)
     gParticles.push( new p() );

 }
