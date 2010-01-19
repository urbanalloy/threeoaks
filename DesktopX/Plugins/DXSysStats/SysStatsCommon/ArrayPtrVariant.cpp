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
#include "ArrayPtrVariant.h"
#include "SysStatsUtils.h"

#ifndef NULL
#define NULL (0)
#endif

#define IN_RANGE(i) (((i) < _count) && ((i) >=0))

template <class I, class B>
ArrayPtrVariant<I, B>::ArrayPtrVariant() : _count(0)
{
	coll = new I*[ARRAY_INC];
	size = ARRAY_INC;
}

// Private, should never be called.
template <class I, class B>
ArrayPtrVariant<I, B>::ArrayPtrVariant(const ArrayPtrVariant<I, B>&) : _count(0)
{
}

template <class I, class B>
ArrayPtrVariant<I, B>::~ArrayPtrVariant()
{
	ClearAndDestroy();
	delete coll;
	coll = 0;
}

template <class I, class B>
STDMETHODIMP  ArrayPtrVariant<I, B>::Clear()
{
	_count = 0;

	return S_OK;
}

template <class I, class B>
STDMETHODIMP ArrayPtrVariant<I, B>::ClearAndDestroy()
{
	for (int c=0; c < _count; c++)
	{
		if (coll[c] != NULL)
			coll[c]->Release();
		coll[c] = NULL;
	}
	_count = 0;

	return S_OK;
}

template <class I, class B>
STDMETHODIMP ArrayPtrVariant<I, B>::Add(I* item)
{
	if (item == NULL)
		AtlTrace("Trying to add NULL to arrayptrvariant\n");
	checkCapacity();
	coll[_count++] = item;
	item->AddRef();

	return S_OK;
}

template <class I, class B>
STDMETHODIMP ArrayPtrVariant<I, B>::AddAt(I* item, VARIANT v)
{
	long i = SysStatsUtils::VariantToInteger(&v);

	checkCapacity();
	if ((((i) <= _count) && ((i) >=0)))
	{
		for (int c=_count; c > i; c--)
		{
			coll[c] = coll[c-1];
		}
	}

	coll[i] = item;
	item->AddRef();
	_count++;

	return S_OK;
}

template <class I, class B>
STDMETHODIMP ArrayPtrVariant<I, B>::GetIndex(I* item, short *pRet)
{
	for (int i=0; i<_count; i++)
	{
		if (coll[i] == item)
		{
			*pRet = i;
			return S_OK;
		}
	}

	*pRet = -1;

	return S_OK;
}

template <class I, class B>
STDMETHODIMP ArrayPtrVariant<I, B>::Item(VARIANT v, I **pRet)
{
	long i = SysStatsUtils::VariantToInteger(&v);

	if (IN_RANGE(i))
	{
		*pRet = coll[i];
		(*pRet)->AddRef();
	}
	else
		*pRet = NULL;

	return S_OK;
}

template <class I, class B>
STDMETHODIMP ArrayPtrVariant<I, B>::Remove(VARIANT v)
{
	long i = SysStatsUtils::VariantToInteger(&v);

	I* ret = NULL;

	if (IN_RANGE(i))
	{
		ret = coll[i];
		ret->Release();

		for (int c=i; c < _count-1; c++)
		{
			coll[c] = coll[c+1];
		}

		_count--;
	}

	return S_OK;
}

template <class I, class B>
STDMETHODIMP ArrayPtrVariant<I, B>::Swap(VARIANT v1, VARIANT v2)
{
	long i1 = SysStatsUtils::VariantToInteger(&v1);
	long i2 = SysStatsUtils::VariantToInteger(&v2);

	if (IN_RANGE(i1) && IN_RANGE(i2))
	{
		I* t = coll[i1];
		coll[i1] = coll[i2];
		coll[i2] = t;
	}

	return S_OK;
}

template <class I, class B>
STDMETHODIMP ArrayPtrVariant<I, B>::Count(short *pRet)
{
	*pRet = _count;

	return S_OK;
}

template <class I, class B>
void ArrayPtrVariant<I, B>::checkCapacity()
{
	if (_count >= size)
	{
		I **newColl = new I*[size+ARRAY_INC];
		for (int i=0; i<size; i++)
			newColl[i] = coll[i];
		delete[] coll;
		coll = newColl;
		size += ARRAY_INC;
	}
}