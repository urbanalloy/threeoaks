///////////////////////////////////////////////////////////////////////////////////////////////
//
// Canvas Plugin for DesktopX
//
// Copyright (c) 2008-2009, Three Oaks Crossing
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CSSColorParser.h"

#include <sstream>
#include <iomanip>
#include <regex>

map<string, string> CCSSColorParser::simple_colors;

CCSSColorParser::CCSSColorParser()
{
	static bool map_init = false;

	// Populate the map on first instance
	if (!map_init)
	{
		map_init = true;

		simple_colors["transparent"] = "rgba(0,0,0,0)";
		simple_colors["aliceblue"] = "f0f8ff";
		simple_colors["antiquewhite"] = "faebd7";
		simple_colors["aqua"] = "00ffff";
		simple_colors["aquamarine"] = "7fffd4";
		simple_colors["azure"] = "f0ffff";
		simple_colors["beige"] = "f5f5dc";
		simple_colors["bisque"] = "ffe4c4";
		simple_colors["black"] = "000000";
		simple_colors["blanchedalmond"] = "ffebcd";
		simple_colors["blue"] = "0000ff";
		simple_colors["blueviolet"] = "8a2be2";
		simple_colors["brown"] = "a52a2a";
		simple_colors["burlywood"] = "deb887";
		simple_colors["cadetblue"] = "5f9ea0";
		simple_colors["chartreuse"] = "7fff00";
		simple_colors["chocolate"] = "d2691e";
		simple_colors["coral"] = "ff7f50";
		simple_colors["cornflowerblue"] = "6495ed";
		simple_colors["cornsilk"] = "fff8dc";
		simple_colors["crimson"] = "dc143c";
		simple_colors["cyan"] = "00ffff";
		simple_colors["darkblue"] = "00008b";
		simple_colors["darkcyan"] = "008b8b";
		simple_colors["darkgoldenrod"] = "b8860b";
		simple_colors["darkgray"] = "a9a9a9";
		simple_colors["darkgreen"] = "006400";
		simple_colors["darkgrey"] = "a9a9a9";
		simple_colors["darkkhaki"] = "bdb76b";
		simple_colors["darkmagenta"] = "8b008b";
		simple_colors["darkolivegreen"] = "556b2f";
		simple_colors["darkorange"] = "ff8c00";
		simple_colors["darkorchid"] = "9932cc";
		simple_colors["darkred"] = "8b0000";
		simple_colors["darksalmon"] = "e9967a";
		simple_colors["darkseagreen"] = "8fbc8f";
		simple_colors["darkslateblue"] = "483d8b";
		simple_colors["darkslategray"] = "2f4f4f";
		simple_colors["darkslategrey"] = "2f4f4f";
		simple_colors["darkturquoise"] = "00ced1";
		simple_colors["darkviolet"] = "9400d3";
		simple_colors["deeppink"] = "ff1493";
		simple_colors["deepskyblue"] = "00bfff";
		simple_colors["dimgray"] = "696969";
		simple_colors["dimgrey"] = "696969";
		simple_colors["dodgerblue"] = "1e90ff";
		simple_colors["feldspar"] = "d19275";
		simple_colors["firebrick"] = "b22222";
		simple_colors["floralwhite"] = "fffaf0";
		simple_colors["forestgreen"] = "228b22";
		simple_colors["fuchsia"] = "ff00ff";
		simple_colors["gainsboro"] = "dcdcdc";
		simple_colors["ghostwhite"] = "f8f8ff";
		simple_colors["gold"] = "ffd700";
		simple_colors["goldenrod"] = "daa520";
		simple_colors["gray"] = "808080";
		simple_colors["grey"] = "808080";
		simple_colors["green"] = "008000";
		simple_colors["greenyellow"] = "adff2f";
		simple_colors["honeydew"] = "f0fff0";
		simple_colors["hotpink"] = "ff69b4";
		simple_colors["indianred "] = "cd5c5c";
		simple_colors["indigo "] = "4b0082";
		simple_colors["ivory"] = "fffff0";
		simple_colors["khaki"] = "f0e68c";
		simple_colors["lavender"] = "e6e6fa";
		simple_colors["lavenderblush"] = "fff0f5";
		simple_colors["lawngreen"] = "7cfc00";
		simple_colors["lemonchiffon"] = "fffacd";
		simple_colors["lightblue"] = "add8e6";
		simple_colors["lightcoral"] = "f08080";
		simple_colors["lightcyan"] = "e0ffff";
		simple_colors["lightgoldenrodyellow"] = "fafad2";
		simple_colors["lightgray"] = "d3d3d3";
		simple_colors["lightgrey"] = "d3d3d3";
		simple_colors["lightgreen"] = "90ee90";
		simple_colors["lightpink"] = "ffb6c1";
		simple_colors["lightsalmon"] = "ffa07a";
		simple_colors["lightseagreen"] = "20b2aa";
		simple_colors["lightskyblue"] = "87cefa";
		simple_colors["lightslateblue"] = "8470ff";
		simple_colors["lightslategrey"] = "778899";
		simple_colors["lightslategray"] = "778899";
		simple_colors["lightsteelblue"] = "b0c4de";
		simple_colors["lightyellow"] = "ffffe0";
		simple_colors["lime"] = "00ff00";
		simple_colors["limegreen"] = "32cd32";
		simple_colors["linen"] = "faf0e6";
		simple_colors["magenta"] = "ff00ff";
		simple_colors["maroon"] = "800000";
		simple_colors["mediumaquamarine"] = "66cdaa";
		simple_colors["mediumblue"] = "0000cd";
		simple_colors["mediumorchid"] = "ba55d3";
		simple_colors["mediumpurple"] = "9370d8";
		simple_colors["mediumseagreen"] = "3cb371";
		simple_colors["mediumslateblue"] = "7b68ee";
		simple_colors["mediumspringgreen"] = "00fa9a";
		simple_colors["mediumturquoise"] = "48d1cc";
		simple_colors["mediumvioletred"] = "c71585";
		simple_colors["midnightblue"] = "191970";
		simple_colors["mintcream"] = "f5fffa";
		simple_colors["mistyrose"] = "ffe4e1";
		simple_colors["moccasin"] = "ffe4b5";
		simple_colors["navajowhite"] = "ffdead";
		simple_colors["navy"] = "000080";
		simple_colors["oldlace"] = "fdf5e6";
		simple_colors["olive"] = "808000";
		simple_colors["olivedrab"] = "6b8e23";
		simple_colors["orange"] = "ffa500";
		simple_colors["orangered"] = "ff4500";
		simple_colors["orchid"] = "da70d6";
		simple_colors["palegoldenrod"] = "eee8aa";
		simple_colors["palegreen"] = "98fb98";
		simple_colors["paleturquoise"] = "afeeee";
		simple_colors["palevioletred"] = "d87093";
		simple_colors["papayawhip"] = "ffefd5";
		simple_colors["peachpuff"] = "ffdab9";
		simple_colors["peru"] = "cd853f";
		simple_colors["pink"] = "ffc0cb";
		simple_colors["plum"] = "dda0dd";
		simple_colors["powderblue"] = "b0e0e6";
		simple_colors["purple"] = "800080";
		simple_colors["red"] = "ff0000";
		simple_colors["rosybrown"] = "bc8f8f";
		simple_colors["royalblue"] = "4169e1";
		simple_colors["saddlebrown"] = "8b4513";
		simple_colors["salmon"] = "fa8072";
		simple_colors["sandybrown"] = "f4a460";
		simple_colors["seagreen"] = "2e8b57";
		simple_colors["seashell"] = "fff5ee";
		simple_colors["sienna"] = "a0522d";
		simple_colors["silver"] = "c0c0c0";
		simple_colors["skyblue"] = "87ceeb";
		simple_colors["slateblue"] = "6a5acd";
		simple_colors["slategray"] = "708090";
		simple_colors["slategrey"] = "708090";
		simple_colors["snow"] = "fffafa";
		simple_colors["springgreen"] = "00ff7f";
		simple_colors["steelblue"] = "4682b4";
		simple_colors["tan"] = "d2b48c";
		simple_colors["teal"] = "008080";
		simple_colors["thistle"] = "d8bfd8";
		simple_colors["tomato"] = "ff6347";
		simple_colors["turquoise"] = "40e0d0";
		simple_colors["violet"] = "ee82ee";
		simple_colors["violetred"] = "d02090";
		simple_colors["wheat"] = "f5deb3";
		simple_colors["white"] = "ffffff";
		simple_colors["whitesmoke"] = "f5f5f5";
		simple_colors["yellow"] = "ffff00";
		simple_colors["yellowgreen"] = "9acd32";
		// User colors
		simple_colors["threeddarkshadow"] = "404040";
	}
}

CCSSColorParser::~CCSSColorParser() {}

void CCSSColorParser::clampColor(RGBAColor* color)
{
	// Clamp values
	color->r = (color->r < 0) ? 0 : ((color->r > 255) ? 255 : color->r);
	color->g = (color->g < 0) ? 0 : ((color->g > 255) ? 255 : color->g);
	color->b = (color->b < 0) ? 0 : ((color->b > 255) ? 255 : color->b);
	color->a = (color->a < 0) ? 0 : ((color->a > 1) ? 1 : color->a);

	// DesktopX workaround
	// DesktopX always use pink to draw transparent surfaces, even in per-pixel mode.
	// rgb(255, 0, 255) will be changed to rgb(255, 1, 255)
	if (color->r == 255 && color->g == 0 && color->b == 255)
		color->g = 1;
}

#define CLAMP_VALUE(val) \
	if (val < 0) \
	val = 0; \
	else if (val > 100) \
	val = 255; \
	else \
	val *= 255.0/100.0; 

//////////////////////////////////////////////////////////////////////////
// h is [0;360], s and v are [0;1]
bool CCSSColorParser::HSVtoRGB(int h, double s, double v, RGBAColor* color)
{
	double r, g, b;
	if (v == 0) 
	{
		r = 0;
		g = 0;
		b = 0;
	} 
	else if (s == 0) 
	{
		r = v;
		g = v;
		b = v;
	}
	else
	{
		const double hf = h/60.0;
		const int    i  = (int) floor(hf);
		const double f  = hf - i;
		const double pv = v * (1 - s);
		const double qv = v * (1 - s * f);
		const double tv = v * (1 - s * (1 - f));
		
		switch(i)                 
		{                                          
			// red is the dominant color 
			case 0:                                    
				r = v;                                   
				g = tv;                                  
				b = pv;                                  
				break;                                   

			// green is the dominant color
			case 1:                                    
				r = qv;                                  
				g = v;                                   
				b = pv;                                  
				break;                                   
			case 2:                                    
				r = pv;                                  
				g = v;                                   
				b = tv;                                  
				break;    

			// blue is the dominant color 
			case 3:                                    
				r = pv;                                  
				g = qv;                                  
				b = v;                                   
				break;                                   
			case 4:                                    
				r = tv;                                  
				g = pv;                                  
				b = v;                                   
				break;  

			// red is the dominant color 
			case 5:                                    
				r = v;                                   
				g = pv;                                  
				b = qv;                                  
				break;    

			// just in case we overshoot on our math by a little, we put these here.
			// since its a switch it won't slow us down at all to put these here. 
			case 6:                                    
				r = v;                                   
				g = tv;                                  
				b = pv;                                  
				break;                                   
			case -1:                                   
				r = v;                                   
				g = pv;                                  
				b = qv;                                  
				break;   

			// the color is not defined, we should throw an error. 
			default:                                   
				return false;
				break;   
		}
	}                                          
	                                              
	color->r = (int)(r * 255.0f);
	color->g = (int)(g * 255.0f);
	color->b = (int)(b * 255.0f);

	return true;
}


bool CCSSColorParser::parseColor(string color_string, RGBAColor* color)
{
	bool match;

	// strip any leading #
	if (color_string.compare(0, 1, "#") == 0)
		color_string = color_string.substr(1, 6);

	// remove all spaces and convert to lowercase
	color_string.erase(remove_if(color_string.begin(), color_string.end(), isspace), color_string.end());
	transform(color_string.begin(), color_string.end(), color_string.begin(), tolower);

	// Try matching with simple colors
	map<string, string>::iterator iter = simple_colors.find(color_string);
	if (iter != simple_colors.end())
			color_string = iter->second; // replace color_string by value

	//////////////////////////////////////////////////////////////////////////
	// Try to match with rgba(reg,green,blue,alpha)	
	tr1::smatch rgba_match;
	tr1::regex rgba_exp("rgba\\(([-+]?[0-9]*)([%]?),\\s*([-+]?[0-9]*)([%]?),\\s*([-+]?[0-9]*)([%]?),\\s*([-+]?[0-9]*\\.?[0-9]+)\\)");
	match = tr1::regex_match(color_string, rgba_match, rgba_exp);

	if (match)
	{
		double r = ToNumber<double>(rgba_match[1]);
		double g = ToNumber<double>(rgba_match[3]);
		double b = ToNumber<double>(rgba_match[5]);		

		bool isPercent = false;

		bool percent1, percent2, percent3;
		(string(rgba_match[2]).length() > 0) ? percent1 = true : percent1 = false;
		(string(rgba_match[4]).length() > 0) ? percent2 = true : percent2 = false;
		(string(rgba_match[6]).length() > 0) ? percent3 = true : percent3 = false;

		if (percent1 || percent2 || percent3)
			isPercent = true;

		if (!(percent1 && percent2 && percent3) && isPercent)
			return false;

		if (isPercent) {
			CLAMP_VALUE(r);
			CLAMP_VALUE(g);
			CLAMP_VALUE(b);
		}

		color->r = r;
		color->g = g;
		color->b = b;

		color->a = ToNumber<float>(rgba_match[7]);

		clampColor(color);

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// Try to match with rgb(red,green,blue)
	tr1::smatch rgb_match;
	tr1::regex rgb_exp("rgb\\(([-+]?[0-9]*)([%]?),\\s*([-+]?[0-9]*)([%]?),\\s*([-+]?[0-9]*)([%]?)\\)");
	match = tr1::regex_match(color_string, rgb_match, rgb_exp);

	if (match)
	{
		double r = ToNumber<double>(rgb_match[1]);
		double g = ToNumber<double>(rgb_match[3]);
		double b = ToNumber<double>(rgb_match[5]);

		bool isPercent = false;

		bool percent1, percent2, percent3;
		(string(rgb_match[2]).length() > 0) ? percent1 = true : percent1 = false;
		(string(rgb_match[4]).length() > 0) ? percent2 = true : percent2 = false;
		(string(rgb_match[6]).length() > 0) ? percent3 = true : percent3 = false;

		if (percent1 || percent2 || percent3)
			isPercent = true;

		if (!(percent1 && percent2 && percent3) && isPercent)
			return false;

		if (isPercent) {
			CLAMP_VALUE(r);
			CLAMP_VALUE(g);
			CLAMP_VALUE(b);
		}
		
		color->r = r;
		color->g = g;
		color->b = b;
		color->a = 1;

		clampColor(color);

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// Try to match with hsla(hue, saturation, lightness, alpha)
	tr1::smatch hsla_match;
	tr1::regex hsla_exp("hsla\\(([-+]?[0-9]*\\.?[0-9]+),\\s*([-+]?[0-9]*\\.?[0-9])[%],\\s*([-+]?[0-9]*\\.?[0-9])[%],\\s*([-+]?[0-9]*\\.?[0-9]+)\\)");
	match = tr1::regex_match(color_string, hsla_match, hsla_exp);

	if (match)
	{
		int h = ToNumber<int>(hsla_match[1]);
		double s = ToNumber<double>(hsla_match[2]);
		double v = ToNumber<double>(hsla_match[3]);		

		// check values
		if (s < 0) s = 0; if (s > 100) s = 100;
		if (v < 0) v = 0; if (v > 100) v = 100;

		h = ((h % 360) + 360) % 360;

		bool ret = HSVtoRGB(h, s/100.0f, v/100.0f, color);
		color->a = ToNumber<float>(hsla_match[4]);

		clampColor(color);

		return ret;
	}

	//////////////////////////////////////////////////////////////////////////
	// Try to match with hsl(hue, saturation, lightness)
	tr1::smatch hsl_match;
	tr1::regex hsl_exp("hsl\\(([-+]?[0-9]*\\.?[0-9]+),\\s*([-+]?[0-9]*\\.?[0-9])[%],\\s*([-+]?[0-9]*\\.?[0-9])[%]\\)");
	match = tr1::regex_match(color_string, hsl_match, hsl_exp);

	if (match)
	{
		int h = ToNumber<int>(hsl_match[1]);
		double s = ToNumber<double>(hsl_match[2]);
		double v = ToNumber<double>(hsl_match[3]);		

		// check values
		if (s < 0) s = 0; if (s > 100) s = 100;
		if (v < 0) v = 0; if (v > 100) v = 100;

		h = ((h % 360) + 360) % 360;
	
		bool ret = HSVtoRGB(h, s/100.0f, v/100.0f, color);
		color->a = 1;

		clampColor(color);

		return ret;
	}

	//////////////////////////////////////////////////////////////////////////
	// Try to match with #rrggbb
	tr1::smatch hex_match;
	tr1::regex hex_exp("([a-fA-F0-9]{2})([a-fA-F0-9]{2})([a-fA-F0-9]{2})");
	match = tr1::regex_match(color_string, hex_match, hex_exp);

	if (match)
	{
		color->r = HexToInt<int>(hex_match[1]);
		color->g = HexToInt<int>(hex_match[2]);
		color->b = HexToInt<int>(hex_match[3]);
		color->a = 1;

		clampColor(color);

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// Try to match with #rgb
	tr1::smatch hex2_match;
	tr1::regex hex2_exp("([a-fA-F0-9]{1})([a-fA-F0-9]{1})([a-fA-F0-9]{1})");
	match = tr1::regex_match(color_string, hex2_match, hex2_exp);

	if (match)
	{
		color->r = SingleHexToInt<int>(hex2_match[1]);
		color->g = SingleHexToInt<int>(hex2_match[2]);
		color->b = SingleHexToInt<int>(hex2_match[3]);
		color->a = 1;

		clampColor(color);

		return true;
	}

	// No match found!
	return false;
}

string CCSSColorParser::toString(RGBAColor* color)
{
	char str[100];
	if (color->a == 1)
		sprintf_s(str, "#%02x%02x%02x", (int)color->r, (int)color->g, (int)color->b);
	else if (color->a == 0)
		sprintf_s(str, "rgba(%.f, %.f, %.f, 0.0)", color->r, color->g, color->b);
	else
		sprintf_s(str, "rgba(%.f, %.f, %.f, %.05f)", color->r, color->g, color->b, color->a);	

	return string(str);
}
