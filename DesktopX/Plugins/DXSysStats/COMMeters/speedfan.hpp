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

/////////////////////////////////////////////////////////////////////////////

#ifndef SPEEDFAN_HPP
#define SPEEDFAN_HPP

#include <windows.h>
#include <string>

namespace sf {

template <typename T>
void flush(const T & val)
{
    FlushViewOfFile(&val, sizeof(val));
}

class sf
{
private:
    #pragma pack(push, 1)
        struct data_t
        {
			short version;
			short flags;
			int MemSize;
			void* handle;
			short NumTemps;
			short NumFans;
			short NumVolts;
			int temps[32];
			int fans[32];
			int volts[32];
		};
    #pragma pack(pop)

    data_t * data;
    HANDLE   sfMemory;

public:
    sf() : data(0), sfMemory(0)
    {
    }

    ~sf()
    {
        close();
    }

    bool open()
    {
        if (opened()) 
            return true;

        // try to open shared memory
        if (sfMemory == 0)
            sfMemory = OpenFileMapping(FILE_MAP_READ, FALSE, "SFSharedMemory_ALM");

        if (sfMemory == 0)
            return false;

        data = (data_t *)MapViewOfFile(sfMemory, FILE_MAP_READ, 0, 0, 0);

        return opened();
    }

    bool opened() const { return data != 0; }

    void close()
    {
        if (data != 0)
        {
            UnmapViewOfFile(data);
            data = 0;
        }

        if (sfMemory != 0)
        {
            CloseHandle(sfMemory);
            sfMemory = 0;
        }
    }

    short version() const { return data->version; }
	short flags() const { return data->flags; }
	int memSize() const { return data->MemSize; }
	short numTemps() const { return data->NumTemps; }
	short numFans() const { return data->NumFans; }
	short numVolts() const { return data->NumVolts; }
	int temp(int index) { return index < data->NumTemps ? data->temps[index] : -1; }
	int fan(int index)  { return index < data->NumFans ? data->fans[index] : -1; }
	int volt(int index) { return index < data->NumVolts ? data->volts[index] : -1; }
};

}   //namespace sf

#endif  //SPEEDFAN_HPP
