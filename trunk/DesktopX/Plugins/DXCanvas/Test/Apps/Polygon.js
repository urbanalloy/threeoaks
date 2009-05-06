// Small cube thing done by Marcus Engene (myfirstname@mylastname.se)

var ctx;

var polygons = new Array(
    // 
    -100, 100, 100,
    100, 100, 100,
    100, -100, 100,
    -100, -100, 100,
    0, 0, 255, // lightsource vector
    // 
    -100, 100, -100,
    100, 100, -100,
    100, -100, -100,
    -100, -100, -100,
    0, 0, -255, // lightsource vector
    // 
    100, 100, 100,
    100, 100, -100,
    100, -100, -100,
    100, -100, 100,
    255, 0, 0,
    // 
    -100, 100, 100,
    -100, 100, -100,
    -100, -100, -100,
    -100, -100, 100,
    -255, 0, 0,
    // 
    -100, -100, 100,
    100, -100, 100,
    100, -100, -100,
    -100, -100, -100,
    0, -255, 0,
    // 
    -100, 100, 100,
    100, 100, 100,
    100, 100, -100,
    -100, 100, -100,
    0, 255, 0
);

var polygons_rot = new Array (polygons.length);

var xyrot = 0.0;
var xzrot = 0.0;
var yzrot = 0.0;

var width = 420;
var height = 356;


function object_onScriptEnter() {
    canvas.width = 600;
    canvas.height = 400;

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


function init() 
{    
    ctx = canvas.getContext('2d');
    
    Object.SetTimer(1, 60);
}
    
function draw(){   
    
    var xadd = (width / 2);
    var yadd = (height / 2);

    
    xyrot += 0.05 / 2;
    xzrot += 0.04 / 2;
    yzrot += 0.07 / 2;
    
    // xy rot
    // x' = cos(g) * x - sin(g) * y;
    // y' = cos(g) * y + sin(g) * x;
    
    // xzrot
    // x' = cos(g) * x - sin(g) * z;
    // z' = cos(g) * z + sin(g) * x;
    
    // yzrot
    // z' = cos(g) * z - sin(g) * y;
    // y' = cos(g) * y + sin(g) * z;
    
    for (i=0; i<polygons.length; ) {
        // xy rot
        x = polygons[i+0];
        y = polygons[i+1];
        z = polygons[i+2];
        polygons_rot[i+0] = Math.cos(xyrot) * x - Math.sin(xyrot) * y;
        polygons_rot[i+1] = Math.cos(xyrot) * y + Math.sin(xyrot) * x;
        polygons_rot[i+2] = z;
        
        // xzrot
        x = polygons_rot[i+0];
        y = polygons_rot[i+1];
        z = polygons_rot[i+2];
        polygons_rot[i+0] = Math.cos(xzrot) * x - Math.sin(xzrot) * z;
        polygons_rot[i+1] = y; // y untouched
        polygons_rot[i+2] = Math.cos(xzrot) * z + Math.sin(xzrot) * x;

        // yzrot
        x = polygons_rot[i+0];
        y = polygons_rot[i+1];
        z = polygons_rot[i+2];
        polygons_rot[i+0] = x;
        polygons_rot[i+1] = Math.cos(yzrot) * y + Math.sin(yzrot) * z;
        polygons_rot[i+2] = Math.cos(yzrot) * z - Math.sin(yzrot) * y;
        
        // perspective
        x = polygons_rot[i+0];
        y = polygons_rot[i+1];
        z = polygons_rot[i+2];
        polygons_rot[i+0] = (x * 800) / (800 - z);
        polygons_rot[i+1] = (y * 800) / (800 - z);
        polygons_rot[i+2] = z;
                
        
        i += 3;
    }
   
    ctx.clearRect (0,0, width, height);    
    
    ctx.fillStyle = "rgb(0,0,255)";
    for (i=0; i<polygons_rot.length; ) {
    
        //if (i != 0) break;
        intensity = Math.round(polygons_rot[i+14]);
        if (intensity > 255) intensity = 255;
        if (intensity < 32) {
            i+=15;
            continue;
        }
        fillstr = "rgba(0,0," + intensity.toString() + ",1)";
        ctx.lineWidth = 0;
        ctx.fillStyle = fillstr;
        ctx.strokeStyle = fillstr;
        ctx.beginPath();
        ctx.moveTo (polygons_rot[i] + xadd, polygons_rot[i+1] + yadd);
        ctx.lineTo (polygons_rot[i+3] + xadd, polygons_rot[i+3+1] + yadd);
        ctx.lineTo (polygons_rot[i+6] + xadd, polygons_rot[i+6+1] + yadd);
        ctx.lineTo (polygons_rot[i+9] + xadd, polygons_rot[i+9+1] + yadd);
        ctx.lineTo (polygons_rot[i] + xadd, polygons_rot[i+1] + yadd);

        ctx.fill();
        i += 15;
    }
    
}
