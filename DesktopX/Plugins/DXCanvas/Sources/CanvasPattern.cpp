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

#include "stdafx.h"
#include "CanvasPattern.h"


void CCanvasPattern::setExtend(cairo_extend_t extend)
{
	if (pattern)
		cairo_pattern_set_extend(pattern, extend);
}

void CCanvasPattern::setPattern(cairo_pattern_t* pattern)
{
	this->pattern = pattern;
}

void CCanvasPattern::apply(cairo_t *context)
{
	if (pattern)
		cairo_set_source(context, pattern);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCanvasPattern::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICanvasPattern
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ICanvasPattern
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
