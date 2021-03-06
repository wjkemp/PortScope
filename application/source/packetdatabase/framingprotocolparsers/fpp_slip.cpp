/*  fpp_slip.cpp
 *
 *  Copyright (C) 2012 Willem Kemp <http://www.thenocturnaltree.com>
 *  All rights reserved.
 *
 *  This file is part of PortScope.
 *
 *  PortScope is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PortScope is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PortScope. If not, see http://www.gnu.org/licenses/.
 *
 */
#include "fpp_slip.h"


//-----------------------------------------------------------------------------
#define SLIP_END             0xC0
#define SLIP_ESC             0xDB
#define SLIP_ESC_END         0xDC
#define SLIP_ESC_ESC         0xDD



//-----------------------------------------------------------------------------
fpp_Slip::fpp_Slip(PacketFlags flags, size_t bufferLength) :
    _flags(flags),
    _bufferLength(bufferLength),
    _index(0),
    _inEscape(false)
{
    _buffer = new unsigned char[_bufferLength];
}


//-----------------------------------------------------------------------------
fpp_Slip::~fpp_Slip()
{
    delete [] _buffer;
}


//-----------------------------------------------------------------------------
std::list<Packet*> fpp_Slip::parse(const unsigned char* data, size_t length)
{
    std::list<Packet*> result;
    const unsigned char* src = (const unsigned char*)data;

    for (size_t i=0; i < length; ++i) {

        // In Escape
        if (_inEscape) {

            switch (src[i]) {
                case SLIP_ESC_ESC: {
                    _buffer[_index++] = SLIP_ESC;
                } break;

                case SLIP_ESC_END: {
                    _buffer[_index++] = SLIP_END;
                } break;

                // Unexpected character
                default: {
                    _index = 0;

                    // Error
                } break;
            }

            _inEscape = false;

        } else {

            switch (src[i]) {
                case SLIP_ESC: {
                    _inEscape = true;
                } break;

                case SLIP_END: {
                    if (_index != 0) {
                        result.push_back(new Packet(_buffer, _index, _flags));
                        _index = 0;
                    }
                } break;

                default: {
                    if (_index == _bufferLength) {
                        _index = 0;
                        // Error
                    } else {
                        _buffer[_index++] = src[i];
                    }
                } break;
            }
        }
    }

    return result;
}
