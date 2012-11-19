/*  packetlayerinfo.h
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
#ifndef __PACKETLAYERINFO_H__
#define __PACKETLAYERINFO_H__


#include <vector>
#include "packetinfo.h"
#include "packetfieldinfo.h"


class PacketLayerInfo : public PacketInfo
{
public:
    PacketLayerInfo(const std::string& info, int layerIndex);
    PacketLayerInfo(size_t offset, size_t length, const std::string& info, int layerIndex);
    ~PacketLayerInfo();

    void addFieldInfo(PacketFieldInfo* fieldInfo);

    std::string info() const;
    int layerIndex() const;
    int fieldCount() const;
    PacketFieldInfo* fieldAt(int index) const;

private:
    std::string _info;
    int _layerIndex;
    std::vector<PacketFieldInfo*> _fields;
};


#endif
