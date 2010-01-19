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
#include "ArrayPtr.h"

#ifndef NULL
#define NULL (0)
#endif

#define IN_RANGE(i) (((i) < _count) && ((i) >=0))

template <class I>
ArrayPtr<I>::ArrayPtr() : _count(0)
{
	coll = new I*[ARRAY_INC];
	size = ARRAY_INC;
}

// Private, should never be called.
template <class I>
ArrayPtr<I>::ArrayPtr(const ArrayPtr<I>&) : _count(0)
{
}

template <class I>
ArrayPtr<I>::~ArrayPtr()
{
	delete coll;
}

template <class I>
void ArrayPtr<I>::clear()
{
	_count = 0;
}

template <class I>
void ArrayPtr<I>::clearAndDestroy()
{
	for (int c=0; c < _count; c++)
	{
		delete coll[c];
		coll[c] = NULL;
	}
	_count = 0;
}

template <class I>
int ArrayPtr<I>::add(I* item)
{
	checkCapacity();
	coll[_count++] = item;

	return _count-1;
}

template <class I>
int ArrayPtr<I>::addAt(int i, I* item)
{
	checkCapacity();
	if ((((i) <= _count) && ((i) >=0)))
	{
		for (int c=_count; c > i; c--)
		{
			coll[c] = coll[c-1];
		}
	}

	coll[i] = item;
	_count++;

	return _count-1;
}

template <class I>
int ArrayPtr<I>::find(const I* item) const
{
	for (int i=0; i<_count; i++)
	{
		if (coll[i] == item)
			return i;
	}

	return -1;
}

template <class I>
I* ArrayPtr<I>::operator[](int i) const
{
	if (IN_RANGE(i))
		return coll[i];
	else
		return NULL;
}

template <class I>
I* ArrayPtr<I>::remove(int i)
{
	I* ret = NULL;

	if (IN_RANGE(i))
	{
		ret = coll[i];

		for (int c=i; c < _count-1; c++)
		{
			coll[c] = coll[c+1];
		}

		_count--;
	}

	return ret;
}

template <class I>
void ArrayPtr<I>::swap(int i1, int i2)
{
	if (IN_RANGE(i1) && IN_RANGE(i2))
	{
		I* t = coll[i1];
		coll[i1] = coll[i2];
		coll[i2] = t;
	}
}

template <class I>
int ArrayPtr<I>::count() const
{
	return _count;
}

template <class I>
void ArrayPtr<I>::checkCapacity()
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