var zdata;

//the following vars can be directly inserted in the function call, I declared here so I can play with them
var caption = 'Sales last week' //the title of the graph
var backgroundColor = 'rgba(255,255,255,0.7)'; //the background color of the graph
var barsCaptions = new Array('M', 'T', 'W', 'T', 'F', 'S', 'S'); //the caption for the group of bars (lines)
var barsNamesColors = new Array(['Beer','#00F'],['Wine','red']); //the names and colors for data bars (columns)

var colors = new Array ('aqua', 'blue', 'gray', 'green', 'lime', 'maroon', 'navy', 'olive', 'orange', 'purple', 'red', 'silver', 'teal', 'yellow');

function object_onScriptEnter() {
    zdata = new Array();

    testing();
}

function object_onScriptExit() {
}

function Object_OnRButtonDown(x, y) {
    testing();
}

function random(val) {
    return Math.floor(Math.random() * val);
}


function testing()
{
	// Clear canvas
	canvas.width = 1

	barsNamesColors[0][1] = colors[random(7)];
	barsNamesColors[1][1] = colors[random(7) + 7];
	fillArray(zdata);
	buildChart(zdata, 100, [10, 15, 20, 25][random(4)], random(100) + 270, random (100) + 190, caption, backgroundColor, barsCaptions, barsNamesColors, '"Trebuchet MS", Helvetica, Arial, sans-serif', 10);
}

function fillArray(theArray)
{
	//here you do what you need to fill the array
	//like get data values from database and
	//assign those values to the "lines & columns" of a multidimensional array
	
	//for testing I will just fill the array with random values
	for (var i = 0; i < 7; i++) {
	
		theArray[i] = new Array();
		theArray[i][0] = Math.floor(Math.random() * 95) + 5; //beer
		theArray[i][1] = Math.floor(Math.random() * 95) + 5; //wine
	}
}


//++++++++++++++++++++++++++++++++++++++++++++++++++

function buildChart(values, maximValue, gridSpace, width, height, title, backColor, barsCap, bars, fontFamily, fontSize, showLegend)
{
	//title: string, the caption of the graph
	//values: the multidimensional array containing the values
	//maximValue: number, the maxim value that the data values can take
	//gridSpace: number, the spacing between horizontal grid lines
	//width: number, the width of the resulting graph
	//height: number, the height of the resulting graph
	//backColor: a string representation of the color to fill the background
	//barsCap: array containing the caption for bars groups (lines) [string,string, ...]
	//bars: array containing the names and colors for data bars (columns) [[string,string],[string,string], ...]
		//the following can be omitted, in which case will be setted to default values
	//fontFamily: string, the font to use for texts
	//fontSize: number, the size of the font for texts (caption will be twice the size, and bold-ed)
	//showLegend: boolean, control the visibility of legend (true if ommited)
	
	
	canvas.width = width;
	canvas.height = height;
	
	//prepare to draw graph using a canvas
	var ctx = canvas.getContext("2d");

	ctx.save();
	ctx.beginPath();
	var pattern = ctx.createLinearGradient(0, 0, 0, height);
	pattern.addColorStop(0.5, "rgba(255, 255, 255, 1)");
	pattern.addColorStop(0.8, "rgba(255, 255, 255, 0.6)");
	pattern.addColorStop(1, "rgba(0, 0, 0, 0.4)");
	ctx.fillStyle = pattern;
	ctx.rect(0, 0, width, height);
	ctx.fill();
	ctx.restore();

	ctx.scale(0.96, 0.96);
	ctx.translate(5, 5);
	
	//we are ready to draw
	
	//grid
	ctx.save();
	ctx.beginPath();
	for (var i = canvas.height + 0.5; i > 0; i -= gridSpace * canvas.height / maximValue)
	{
		ctx.moveTo(0, parseInt(i) + 0.5);
		ctx.lineTo(canvas.width - 0.5, parseInt(i) + 0.5);
	}
	ctx.strokeStyle = 'rgba(0,0,0,0.25)';
	ctx.stroke();
	
	//bars
	ctx.scale(canvas.width / (values.length * (values[0].length + 1) * 10 + 10), canvas.height / maximValue);
	ctx.translate(0, maximValue);
	for (var x = 0; x < values.length; x++)
		for (var y = 0; y < values[0].length; y++)
		{
			ctx.fillStyle = bars[y][1];
			ctx.fillRect( x * 10 * (values[0].length + 1) + y * 10 + 10, 0, 10, - Number(values[x][y]) );
		}
	
	//axes
	ctx.restore();
	ctx.beginPath();
	ctx.moveTo(0.5,0);
	ctx.lineTo(0.5, canvas.height - 0.5);
	ctx.lineTo(canvas.width - 0.5, canvas.height - 0.5);
	ctx.strokeStyle = '#000';
	ctx.stroke();
	
	//done with canvas drawing, now add text objects
	/*
	//if omitted, set to default
	if (!fontFamily) fontFamily = '';
	if (!fontSize) fontSize = 12;
	if (showLegend == null) showLegend = true;
	
	//caption
	var t = new Text();
	t.style.fontFamily = fontFamily;
	t.size = fontSize * 2;
	t.style.fontWeight = 'bold';
	t.data = title;
	t.vOffset = height * 0.05 + t.height / 2;
	t.hOffset = (width - t.width) / 2;
	gf.appendChild(t);
	
	//values
	var val = 0;
	for (var i = 0; i <= maximValue ; i += gridSpace)
	{
		var t = new Text();
		t.style.fontFamily = fontFamily;
		t.size = fontSize;
		t.data = val;
		t.vOffset = c.vOffset + c.height + t.height / 3 - i * c.height / maximValue;
		t.hAlign = 'right';
		t.hOffset = c.hOffset - 4;
		gf.appendChild(t);
		val += gridSpace;
	}
	
	//bars caption
	var spc = c.width / (values.length * 3 + 1);
	for (var i = 0; i < values.length; i++)
	{
		var t = new Text();
		t.style.fontFamily = fontFamily;
		t.size = fontSize;
		t.data = barsCap[i];
		t.vOffset = c.vOffset + c.height + t.height;
		t.hAlign = 'center';
		t.hOffset = c.hOffset  + i * spc * 3 + spc * 2;
		gf.appendChild(t);
	}
	
	//legend
	if (showLegend)
	{
		var lf = new Frame();
		var dx = 0;
		for (var i = 0; i < bars.length; i++)
		{
			var tc = new Text();
			tc.anchorStyle ='topLeft';
			tc.style.fontFamily = fontFamily;
			tc.size = fontSize;
			tc.data = ' ';
			tc.width = tc.height = tc.height * 0.9;
			tc.style.backgroundColor = bars[i][1];
			tc.hOffset = dx;
			lf.appendChild(tc);
			var t = new Text();
			t.anchorStyle ='topLeft';
			t.style.fontFamily = fontFamily;
			t.size = fontSize;
			t.data = bars[i][0];
			t.hOffset = tc.hOffset + tc.width * 1.3;
			lf.appendChild(t);
			dx += t.hOffset + t.width + tc.width * 0.6;
		}
		lf.hOffset = c.hOffset + c.width - lf.width;
		lf.vOffset = height * 0.11 + lf.height / 2;
		gf.appendChild(lf);
	}*/
}
