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

#pragma once
#include "stdafx.h"

#include <limits>
using namespace std;


#define ADJUST_COORD(dimension, coord) \
	if (dimension < 0) { \
		dimension = abs(dimension); \
		coord -= dimension; }

//////////////////////////////////////////////////////////////////////////
// Parameter validation
#define IS_NAN(var) \
	(!(var == var))

#define IS_INFINITY(var) \
	(var == numeric_limits<double>::infinity() || var == -numeric_limits<double>::infinity())

#define CHECK_INFINITY(var) \
	if (IS_INFINITY(var)) \
		return S_OK;

#define CHECK_NAN(var) \
	if (IS_NAN(var)) \
		return S_OK;

