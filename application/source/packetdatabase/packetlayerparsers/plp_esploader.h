/*  plp_esploader.h
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
#ifndef __PLP_ESPLOADER_H__
#define __PLP_ESPLOADER_H__

#include "packetdatabase/packetlayerparser.h"

class plp_EspLoader : public PacketLayerParser
{
public:
    plp_EspLoader(int layerIndex);
    ~plp_EspLoader();
    std::string parseInfo(const unsigned char* data, size_t length, size_t& layerOffset, size_t& layerLength);
    PacketLayerInfo* parseDetail(const unsigned char* data, size_t length, size_t& layerOffset, size_t& layerLength);

private:
    void parseCmdGetHardwareLabel(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseRspHardwareLabel(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseCmdGetSoftwareLabel(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseRspSoftwareLabel(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseCmdReadMemory(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseRspReadMemory(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseCmdGetTransferInfo(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseRspTransferInfo(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseCmdEraseFlash(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseCmdProgramFlash(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseCmdBlankCheck(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseCmdSetHardwareLabel(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseCmdStartSession(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseCmdVerifyCode(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseCmdPoll(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseRspFlashAcknowledge(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);
    void parseRspAcknowledge(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset);

    const char* commandToText(unsigned short command) const;
    const char* errorCodeToText(unsigned short code) const;
    const char* flashTypeToText(unsigned short type) const;

private:
    int _layerIndex;
};


#endif
