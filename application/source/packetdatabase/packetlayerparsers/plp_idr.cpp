/*  plp_idr.cpp
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
#include "plp_idr.h"

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    unsigned char ratio;
    unsigned char type;
    unsigned char reserved1;
    unsigned char reserved2;
    unsigned char sourceNode;
    unsigned char sourceCode;
    unsigned char destNode;
    unsigned char destCode;
    unsigned short checksum;

} IDR_FOOTER;

#pragma pack(pop)


#define OFFSET(x, y) ((const char*)y - (const char*)x) 



//-----------------------------------------------------------------------------
plp_IDR::plp_IDR(int layerIndex) :
    _layerIndex(layerIndex)
{

}


//-----------------------------------------------------------------------------
plp_IDR::~plp_IDR()
{

}


//-----------------------------------------------------------------------------
std::string plp_IDR::parseInfo(const unsigned char* data, size_t length, size_t& layerOffset, size_t& layerLength)
{
    return std::string();
}


//-----------------------------------------------------------------------------
PacketLayerInfo* plp_IDR::parseDetail(const unsigned char* data, size_t length, size_t& layerOffset, size_t& layerLength)
{    
    size_t footerOffset = layerLength - sizeof(IDR_FOOTER);
    size_t footerLength = sizeof(IDR_FOOTER);

    PacketLayerInfo* packetLayerInfo = new PacketLayerInfo(footerOffset, footerLength, "IDR Layer", _layerIndex);
    IDR_FOOTER* footer = (IDR_FOOTER*)(data + footerOffset);


    // Format
    packetLayerInfo->addFieldInfo(new PacketFieldInfo(packetLayerInfo, footerOffset + OFFSET(footer, &footer->ratio), 1, "Ratio: %d", footer->ratio));
    packetLayerInfo->addFieldInfo(new PacketFieldInfo(packetLayerInfo, footerOffset + OFFSET(footer, &footer->type), 1, "Type: %d", footer->type));
    packetLayerInfo->addFieldInfo(new PacketFieldInfo(packetLayerInfo, footerOffset + OFFSET(footer, &footer->sourceNode), 1, "Source Node: %d", footer->sourceNode));
    packetLayerInfo->addFieldInfo(new PacketFieldInfo(packetLayerInfo, footerOffset + OFFSET(footer, &footer->sourceCode), 1, "Source Code: %d", footer->sourceCode));
    packetLayerInfo->addFieldInfo(new PacketFieldInfo(packetLayerInfo, footerOffset + OFFSET(footer, &footer->destNode), 1, "Destination Node: %d", footer->destNode));
    packetLayerInfo->addFieldInfo(new PacketFieldInfo(packetLayerInfo, footerOffset + OFFSET(footer, &footer->destCode), 1, "Destination Code: %d", footer->destCode));
    packetLayerInfo->addFieldInfo(new PacketFieldInfo(packetLayerInfo, footerOffset + OFFSET(footer, &footer->checksum), 2, "Checksum: %04X", footer->checksum));

    // Adjust
    layerLength -= sizeof(IDR_FOOTER);

    // Return
    return packetLayerInfo;
}
