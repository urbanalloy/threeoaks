function object_onScriptEnter() {
    canvas.width = 300;
    canvas.height = 228;

    var context = canvas.getContext('2d');

    // draw the image onto the canvas
    var image = context.loadImage(Widget.Preference("ScriptFolder").Value + '/tests/images/rhino.jpg');
	context.drawImage(image, 0, 0, 300, 228);

	// get the image data to manipulate
	var input = context.getImageData(0, 0, canvas.width, canvas.height);

	// get an empty slate to put the data into
	var output = context.createImageData(canvas.width, canvas.height);

	// alias some variables for convenience
	var w = input.width, h = input.height;
	var inputData = input.data;
	var outputData = output.data;

	// edge detection
	for (var y = 1; y < h-1; y += 1) {
		for (var x = 1; x < w-1; x += 1) {
			for (var c = 0; c < 3; c += 1) {
			    var i = (y * w + x) * 4 + c;
			    
			    var tmp = 127 - inputData.XXX5(i - w * 4 - 4) - inputData.XXX5(i - w * 4) - inputData.XXX5(i - w * 4 + 4)
			                  - inputData.XXX5(i - 4) + 8 * inputData.XXX5(i) - inputData.XXX5(i + 4)
			                  - inputData.XXX5(i + w * 4 - 4) - inputData.XXX5(i + w * 4) - inputData.XXX5(i + w * 4 + 4);

			    if (tmp < 0)
			        tmp = 0;
			    if (tmp > 255)
			        tmp = 255;
			    			    
			    outputData.XXX6(i, tmp);			    
			}
			outputData.XXX6((y*w + x)*4 + 3, 255); // alpha
		}
	}

	// put the image data back after manipulation
	context.putImageData(output, 0, 0);
}

function object_onScriptExit() {

}
