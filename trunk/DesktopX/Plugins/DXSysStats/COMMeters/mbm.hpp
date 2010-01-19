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
//  File     : mbm.hpp
//  Date     : Octobre 13, 2003
//  Author   : Danny Couture
//      Mail : dcouture@popupsolutions.com
//  Version  : 1.0
//  Desc     : C++ wrapper for MotherBoard Monitor Shared Memory Access
//
#ifndef MBM_HPP
#define MBM_HPP

#include <windows.h>
#include <string>

namespace mbm {

typedef enum { btISA = 0, btSMBus, btVIA686ABus, btDirectIO } bus_t;
typedef enum { smtSMBIntel = 0, smtSMBAMD, smtSMBALi, smtSMBNForce, smtSMBSIS } smb_t;
typedef enum { stUnknown = 0, stTemperature, stVoltage, stFan, stMhz, stPercentage } sensor_t;

template <typename T>
void flush(const T & val)
{
    FlushViewOfFile(&val, sizeof(val));
}

#pragma pack(push, 1)

class index
{
private:
    int           _type;                   // type of sensor
    int           _count;                  // number of sensor for that type

public:
    sensor_t type() const { return (sensor_t)_type; }
    int count()     const { return _count; }
};

class sensor
{
private:
    char           _type;                 // type of sensor
    char           _name[12];             // name of sensor
    char           _padding1[3];          // padding of 3 byte
    double         _current;              // current value
    double         _low;                  // lowest readout
    double         _high;                 // highest readout
    long           _readout;              // total number of readout
    char           _padding2[4];          // padding of 4 byte
    long double    _total;                // total amout of all readouts
    char           _padding3[8];          // padding of 6 byte
    double         _alarm1;               // temp & fan: low alarm; voltage: % off;
    double         _alarm2;               // temp: high alarm

public:
    sensor_t type() const { return (sensor_t)_type; }
    void type(sensor_t type) { _type = (char)type; flush(_type); }

    std::string name() const { return std::string(_name); }
    void name(const std::string & name) { strncpy(_name, name.c_str(), sizeof(_name)); flush(_name); }

    double current() const { return _current; }
    void current(double current) { _current = current; flush(_current); }

    double low() const { return _low; }
    void low(double low) { _low = low; flush(_low); }

    double high() const { return _high; }
    void high(double high) { _high = high; flush(_high); }

    long readout() const { return _readout; }
    void readout(long readout) { _readout = readout; flush(_readout); }

    double alarm1() const { return _alarm1; }
    void alarm1(double alarm1) { _alarm1 = alarm1; flush(_alarm1); }

    double alarm2() const { return _alarm2; }
    void alarm2(double alarm2) { _alarm2 = alarm2; flush(_alarm2); }
};

class info
{
private:
    short          _smbBase;              // SMBus base address
    bus_t          _smbType;              // SMBus/Isa bus used to access chip
    smb_t          _smbCode;              // SMBus sub type, Intel, AMD or ALi
    char           _smbAddr;              // Address of sensor chip on SMBus
    char           _smbName[41];          // Nice name for SMBus
    short          _isaBase;              // ISA base address of sensor chip on ISA
    int            _chipType;             // Chip nr, connects with Chipinfo.ini
    char           _voltageSubType;       // Subvoltage option selected

public:

    short          smbBase()        const { return _smbBase; }
    bus_t          smbType()        const { return _smbType; }
    smb_t          smbCode()        const { return _smbCode; }

    char           smbAddr()        const { return _smbAddr; }
    std::string    smbName()        const { return std::string(_smbName); }
    short          isaBase()        const { return _isaBase; }
    int            chipType()       const { return _chipType; }
    char           voltageSubType() const { return _voltageSubType; }
};

#pragma pack(pop)

class mbm
{
private:
    #pragma pack(push, 1)
        struct data_t
        {
            double         _version;              // version number (example: 51090)
            index          _index[10];            // Sensor index
            sensor         _sensor[100];          // sensor info
            info           _info;                 // misc. info
            unsigned char  _start[41];            // start time
            unsigned char  _current[41];          // current time
            unsigned char  _path[256];            // MBM path
        };
    #pragma pack(pop)

    data_t * data;
    HANDLE   mbmMemory;

public:
    mbm()
        : data(0), mbmMemory(0)
    {
    }

    ~mbm()
    {
        close();
    }

    bool open()
    {
        if (opened()) 
            return true;

        // try to open shared memory
        if (mbmMemory == 0)
            mbmMemory = OpenFileMapping(FILE_MAP_WRITE, FALSE, "$M$B$M$5$S$D$");

        if (mbmMemory == 0)
            return false;

        data = (data_t *)MapViewOfFile(mbmMemory, FILE_MAP_WRITE, 0, 0, 0);

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

        if (mbmMemory != 0)
        {
            CloseHandle(mbmMemory);
            mbmMemory = 0;
        }
    }

    double      version() const { return data->_version; }
    std::string path()    const { return std::string((char*)data->_path); }

    index  *    index()  { return data->_index; }
    sensor *    sensor() { return data->_sensor; }
};

}   //namespace mbm

#endif  //MBM_HPP
