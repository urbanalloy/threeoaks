///////////////////////////////////////////////////////////////////////////////////////////////
//
// Canvas Plugin for DesktopX
//
// Copyright (c) 2008, Three Oaks Crossing
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"

#include <string>
#include <map>

using namespace std;

struct RGBAColor {
	double r;
	double g;
	double b;
	float a;

	RGBAColor()
	{
		r = 0;
		g = 0;
		b = 0;
		a = 1.0;
	}
};

class CCSSColorParser
{
	private:
		static map<string, string> simple_colors;

		void clampColor(RGBAColor* color);
		bool HSVtoRGB(int h, double s, double v, RGBAColor* color);

		template<class T>
		T ToNumber(const std::string &str)
		{
			if(str.size()==0)return 0;
			std::istringstream iss(str);
			T result=0;
			iss>>std::dec>>result;
			return result;
		}

		template<class T>
		T HexToInt(const std::string &str)
		{
			if(str.size()==0)return 0;
			std::istringstream iss(str);
			T result=0;
			iss>>std::hex>>result;
			return result;
		}

		template<class T>
		T SingleHexToInt(const std::string &str)
		{
			if(str.size()==0)return 0;
			string fullHex(str);
			std::istringstream iss(fullHex.append(str));
			T result=0;
			iss>>std::hex>>result;
			return result;
		}

	public:

		CCSSColorParser();
		~CCSSColorParser();

		bool parseColor(string color_string, RGBAColor* color);
		string toString(RGBAColor* color);
};
