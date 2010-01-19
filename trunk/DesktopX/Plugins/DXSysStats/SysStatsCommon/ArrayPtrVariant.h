/*
 * SysStats Widget Framework
 * Copyright (C) 2002-2006 Paul Andrews
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once
#include "APVDummy.h"	// A do-nothing default base class

#define ARRAY_INC 5

template <class I, class Base=APVDummy>
class ArrayPtrVariant : public Base
{
public:
	ArrayPtrVariant();
	virtual ~ArrayPtrVariant();

	STDMETHOD(Add)(I*);
	STDMETHOD(AddAt)(I*, VARIANT);
	STDMETHOD(GetIndex)(I*, short *i);
	STDMETHOD(Item)(VARIANT, I**);
	STDMETHOD(Remove)(VARIANT);
	STDMETHOD(Swap)(VARIANT, VARIANT);
	STDMETHOD(Count)(short*);
	STDMETHOD(Clear)();
	STDMETHOD(ClearAndDestroy)();

private:
	ArrayPtrVariant(const ArrayPtrVariant&);
	virtual void checkCapacity();

	I **coll;
	int _count, size;
};