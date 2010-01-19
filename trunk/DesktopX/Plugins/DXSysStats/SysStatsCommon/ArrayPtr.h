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

#define ARRAY_INC 5

template <class I>
class ArrayPtr
{
public:
	ArrayPtr();
	virtual ~ArrayPtr();

	virtual int add(I*);
	virtual int addAt(int i, I*);
	virtual int find(const I*) const;
	virtual I* operator[](int) const;
	virtual I* remove(int);
	virtual void swap(int, int);
	virtual int count() const;
	virtual void clear();
	virtual void clearAndDestroy();

private:
	ArrayPtr(const ArrayPtr&);
	virtual void checkCapacity();

	I **coll;
	int _count, size;
};