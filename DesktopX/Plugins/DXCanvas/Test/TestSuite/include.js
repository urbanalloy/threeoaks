var CANVAS_X = 50;
var CANVAS_Y = 65;

var CANVAS2_X = 50;
var CANVAS2_Y = 150;

var ERROR_X = 5;
var ERROR_Y = 235;

var CANVAS_WIDTH = 100;
var CANVAS_HEIGHT = 50;

///////////////////////////////////////////////
// DomException Error codes
///////////////////////////////////////////////
var INDEX_SIZE_ERR = 1;
var DOMSTRING_SIZE_ERR = 2;
var HIERARCHY_REQUEST_ERR = 3;
var WRONG_DOCUMENT_ERR = 4;
var INVALID_CHARACTER_ERR = 5;
var NO_DATA_ALLOWED_ERR = 6;
var NO_MODIFICATION_ALLOWED_ERR = 7;
var NOT_FOUND_ERR = 8;
var NOT_SUPPORTED_ERR = 9;
var INUSE_ATTRIBUTE_ERR = 10;
var INVALID_STATE_ERR = 11;
var SYNTAX_ERR = 12;
var INVALID_MODIFICATION_ERR = 13;
var NAMESPACE_ERR = 14;
var INVALID_ACCESS_ERR = 15;
var VALIDATION_ERR = 16;
var TYPE_MISMATCH_ERR = 17;
///////////////////////////////////////////////

// JS exceptions
var OVERFLOW_ERR = 6;
var MISMATCH_ERR = 13;
var WRONG_NUMBER_OF_ARGUMENTS_ERR = 450;


///////////////////////////////////////////////
// Test Info and results
///////////////////////////////////////////////
var STATUS_INVALID = -1;
var STATUS_FAIL = 0;
var STATUS_SUCCESS = 1;
var STATUS_MANUAL = 2;

var test_name = "";
var test_description = "";
var test_expected = "";
var test_results = "";
var test_status = STATUS_INVALID;
///////////////////////////////////////////////

var callback = null;
var ctx = _getCanvas().getContext('2d');

// The context used to show test results
var ctx_test = canvas.getContext('2d');

function _prepareCanvas(name, image, desc)
{
	 ////////////////////////////
	 // Set test status
	 test_name = name;
	 test_description = desc;
	 test_expected = image;
	 ////////////////////////////

	canvas.width = 600;
	canvas.height = 400;

	// Draw info and expected output	
	ctx_test.fillStyle = "white";
	ctx_test.fillRect(0, 0, canvas.width, canvas.height);

	ctx_test.font = "Sans Bold 12";
	ctx_test.fillStyle = "black";
	ctx_test.fillText(name, 60, 17);
	
	if (desc != "") {
		ctx_test.font = "Sans Bold 7";
		ctx_test.fillText(desc, 45, 32);
	}
	
	// Move test context
	var origin = DesktopX.Object("CanvasObjectJS")
	_getCanvas().width = CANVAS_WIDTH;
	_getCanvas().height = CANVAS_HEIGHT;
	DesktopX.Object("CanvasObjectTestSuite").Left = origin.Left + CANVAS_X;
	DesktopX.Object("CanvasObjectTestSuite").Top  = origin.Top + CANVAS_Y;
	DesktopX.Object("CanvasObjectTestSuite").OnTop();
	
	ctx_test.fillStyle = "black";
	ctx_test.fillRect(CANVAS_X-2, CANVAS_Y-2, CANVAS_WIDTH+4, CANVAS_HEIGHT+4); 
	ctx_test.fillRect(CANVAS2_X-2, CANVAS2_Y-2, CANVAS_WIDTH+4, CANVAS_HEIGHT+4); 
	ctx_test.fillStyle = "white";
	ctx_test.fillRect(CANVAS_X, CANVAS_Y, CANVAS_WIDTH, CANVAS_HEIGHT); 
	ctx_test.fillRect(CANVAS2_X, CANVAS2_Y, CANVAS_WIDTH, CANVAS_HEIGHT); 
	
	
	ctx_test.fillStyle = "black";
	ctx_test.font = "Sans Bold 8";
	ctx_test.fillText("Actual Output:", 5, 55);	

	ctx_test.fillText("Expected Output:", 5, 140);	
	if (image != "") {
		var img_expected = ctx_test.loadImage(Widget.Preference("TestsFolder").Value + '/output/' + image);
		ctx_test.drawImage(img_expected, CANVAS2_X, CANVAS2_Y);
	}
}

function _loadImage(name) {
	return ctx.loadImage(Widget.Preference("TestsFolder").Value + '/images/' + name);
}

function registerCallback(callbackFunc) {
  callback = callbackFunc;
}

// The canvas used for testing
function _getCanvas() {
	return DesktopX.ScriptObject("CanvasObjectTestSuite").canvas;
}

// Second canvas (VBScript associated object)
function _getCanvas2() {
    return DesktopX.ScriptObject("CanvasObject").canvas;
}

function _doTest(unattended)
{
	if (!unattended) {
		DesktopX.Object("CanvasObjectTestSuite").Visible = true;
	}

	// Do the test
	try
	{
		test();
	}
	catch (e)
	{
		_fail('Aborted with exception: ' + e.name  + ": " + e.message);
		_fail("Details: " + e.description + " (" + (e.number & 0xFFFF) + ")");
		deferred = false; // cancel any deference
	}
	
	if (!deferred)
		endTest();
}


function _valToString(val)
{
  try {
    if (val === undefined || val === null)
      return '[' + typeof(val) + ']';
    return val.toString() + '[' + typeof(val) + ']';
	} catch (e) {
     return '[' + typeof(val) + ']';
	}
}

var _hex2dec_table = {
	0:0, 1:1, 2:2, 3:3, 4:4, 5:5, 6:6, 7:7, 8:8, 9:9,
	a:10, b:11, c:12, d:13, e:14, f:15,
	A:10, B:11, C:12, D:13, E:14, F:15
};
function _hex2dec(hex)
{
	return _hex2dec_table[hex.charAt(0)]*16 + _hex2dec_table[hex.charAt(1)];
}

var _failed = false;
var _asserted = false;
var deferred = false;
var line = 12;
function _warn(text)
{
	ctx_test.font = "Sans 8";
	ctx_test.fillStyle = "black";

	ctx_test.fillText(text, ERROR_X, ERROR_Y+line);	
	line += 12;
	
	// Save text for callback
	test_results += text + "\n";
}

function _fail(text)
{
	_warn(text);
	_failed = true;
}

function _assert(cond, text)
{
	_asserted = true;
	if (! cond)
		_fail('Failed assertion: ' + text);
}

function _assertSame(a, b, text_a, text_b)
{
	_asserted = true;
	if (a !== b)
		_fail('Failed assertion ' + text_a + ' === ' + text_b +
				' (got ' + _valToString(a) + ', expected ' + _valToString(b) + ')');
}

function _assertDifferent(a, b, text_a, text_b)
{
	_asserted = true;
	if (a === b)
		_fail('Failed assertion ' + text_a + ' !== ' + text_b +
				' (got ' + _valToString(a) + ', expected not ' + _valToString(b) + ')');
}

function _assertEqual(a, b, text_a, text_b)
{
	_asserted = true;
	if (a != b)
		_fail('Failed assertion ' + text_a + ' == ' + text_b +
				' (got ' + _valToString(a) + ', expected ' + _valToString(b) + ')');
}

function _assertMatch(a, b, text_a, text_b)
{
	_asserted = true;
	if (! a.match(b))
		_fail('Failed assertion ' + text_a + ' matches ' + text_b +
				' (got ' + _valToString(a) + ')');
}


var _manual_check = false;

function _requireManualCheck()
{
	_manual_check = true;
}

function _crash()
{
	_fail('Aborted due to predicted crash');
}


var _getImageDataIsPremul = false;
var _getImageDataIsBGRA = false;
function _getPixel(x,y)
{	
	try {
		var imgdata = ctx.getImageData(x, y, 1, 1);
	} catch (e) {
		// probably a security exception caused by having drawn
		// data: URLs onto the canvas
		imgdata = null;			
	}
	if (imgdata)
	{
		// Undo the BGRA flipping
		var rgba = (_getImageDataIsBGRA
			? [imgdata.data.XXX5(2), imgdata.data.XXX5(1), imgdata.data.XXX5(0), imgdata.data.XXX5(3)]
			: [imgdata.data.XXX5(0), imgdata.data.XXX5(1), imgdata.data.XXX5(2), imgdata.data.XXX5(3)]);

		if (!_getImageDataIsPremul)
		    return rgba;

		// Undo the premultiplying
		if (rgba[3] == 0)
		    return [0, 0, 0, 0];
		else {
		    var a = rgba[3] / 255;
		    return [
				Math.round(rgba[0] / a),
				Math.round(rgba[1] / a),
				Math.round(rgba[2] / a),
				rgba[3]
			];
		}
	}
	
	_warn("(Can't test pixel value)");
	_manual_check = true;
	return undefined;
}

function _assertPixel(canvas, x,y, r,g,b,a, pos, colour)
{
	_asserted = true;
	var c = _getPixel(x,y);
	if (c && ! (c[0] == r && c[1] == g && c[2] == b && c[3] == a))
		_fail('Failed assertion: got pixel [' + c + '] at ('+x+','+y+'), expected ['+r+','+g+','+b+','+a+']');
}

function _assertPixelApprox(canvas, x,y, r,g,b,a, pos, colour, tolerance)
{
	_asserted = true;
	var c = _getPixel(x,y);
	if (c)
	{
		var diff = Math.max(Math.abs(c[0]-r), Math.abs(c[1]-g), Math.abs(c[2]-b), Math.abs(c[3]-a));
		if (diff > tolerance)
			_fail('Failed assertion: got pixel [' + c + '] at ('+x+','+y+'), expected ['+r+','+g+','+b+','+a+'] +/- '+tolerance);
	}
}

function endTest()
{	
	ctx_test.font = "Sans Bold 9";
	if (_failed) // test failed
	{
		ctx_test.fillStyle = "red";
		ctx_test.fillText("Fail!", ERROR_X, ERROR_Y);
		
		test_status = STATUS_FAIL;
	}
	else if (_manual_check || !_asserted) // test case explicitly asked for a manual check, or no automatic assertions were performed
	{ 		
		ctx_test.fillStyle = "orange";
		ctx_test.fillText("Cannot automatically verify result!", ERROR_X, ERROR_Y);	
		
		test_status = STATUS_MANUAL;
	}
	else // test succeeded
	{	
		ctx_test.fillStyle = "green";
		ctx_test.fillText("Success!", ERROR_X, ERROR_Y);		
		
		test_status = STATUS_SUCCESS;
	}
	
	if (callback != null) {
		var test_image = Widget.Preference("TestsFolder").Value + '/results/' + test_name + ".png";
						
		_getCanvas().toImage(test_image);
    	callback(test_name, test_description, test_expected, test_name + ".png", test_results, test_status);    	
    } 
}
