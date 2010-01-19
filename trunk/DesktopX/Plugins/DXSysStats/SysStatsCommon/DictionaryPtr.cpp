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
#include <string.h>
#include "ArrayPtr.h"
#include "ArrayPtr.cpp"
#include "DictionaryPtr.h"

static unsigned int hash(const char *key)
{
	int len = strlen(key);
	unsigned int hash;

	for (hash = 0; len; len--, key++)
		/* (31 * hash) will probably be optimized to ((hash << 5) - hash). */
		hash = 31 * hash + *key;

	return hash;
}

template <class K, class V>
DictionaryPtr<K, V>::DictionaryPtr(bool (*pEq)(const K* lhs, const K* rhs))
{
	pTable = new ArrayPtr<Entry>();
	this->pEq = pEq;
}

// Private, should never be called.
template <class K, class V>
DictionaryPtr<K, V>::DictionaryPtr(const DictionaryPtr<K, V>&)
{
}

template <class K, class V>
DictionaryPtr<K, V>::~DictionaryPtr()
{
	delete pTable;
}

template <class K, class V>
void DictionaryPtr<K, V>::clear()
{
	pTable->clear();
}

template <class K, class V>
void DictionaryPtr<K, V>::clearAndDestroy()
{
	pTable->clearAndDestroy();
}

template <class K, class V>
void DictionaryPtr<K, V>::put(K* key, V* value)
{
	remove(key);

	Entry *e = new Entry();
	e->key = key;
	e->value = value;
	pTable->add(e);
}

template <class K, class V>
V* DictionaryPtr<K, V>::get(const K* key) const
{
	int i = getIndex(key);

	if (i >= 0)
		return (*pTable)[i]->value;
	else
		return NULL;
}

template <class K, class V>
bool DictionaryPtr<K, V>::contains(const K* key) const
{
	return getIndex(key) != -1;
}

template <class K, class V>
void DictionaryPtr<K, V>::remove(const K* key)
{
	int i = getIndex(key);

	if (i >= 0)
	{
		delete (*pTable)[i];
		pTable->remove(i);
	}
}

template <class K, class V>
void DictionaryPtr<K, V>::getKeys(K** outArrayPtr) const
{
	for (int i=0; i<pTable->count(); i++)
	{
		Entry *e = (*pTable)[i];
		outArrayPtr[i] = e->key;
	}
}

template <class K, class V>
int DictionaryPtr<K, V>::count() const
{
	return pTable->count();
}

template <class K, class V>
int DictionaryPtr<K, V>::getIndex(const K* key) const
{
	for (int i=0; i<pTable->count(); i++)
	{
		Entry *e = (*pTable)[i];
		if ((*pEq)(e->key, key))
		{
			return i;
		}
	}

	return -1;
}

