///////////////////////////////////////////////////////////////////////////////////////////////
//
// Canvas Plugin for DesktopX
//
// Copyright (c) 2008-2010, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. The name of the author may not be used to endorse or promote products derived from this
//     software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
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
