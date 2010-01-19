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

template <class K, class V>
class DictionaryPtr
{
public:
	DictionaryPtr(bool (*pEq)(const K* lhs, const K* rhs));
	~DictionaryPtr();

	void put(K* key, V* value);
	V* get(const K* key) const;
	bool contains(const K* key) const;
	void remove(const K* key);
	int count() const;
	void clear();
	void clearAndDestroy();
	void getKeys(K** outArray) const;

protected:
	class Entry
	{
	public:
		K* key;
		V* value;
	};

	DictionaryPtr(const DictionaryPtr&);
	int getIndex(const K* key) const;

	ArrayPtr<Entry> *pTable;
	bool (*pEq)(const K* lhs, const K* rhs);
};