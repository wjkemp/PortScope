/*  plp_esploader.cpp
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
#include "plp_esploader.h"

#pragma pack(push)
#pragma pack(1)


#define NTOHS(x) (((x & 0xff) << 8) | ((x >> 8) & 0xff))
#define NTOHL(x) (((x & 0xff) << 24) | (((x >> 8) & 0xff) << 16) | (((x >> 16) & 0xff) << 8) | ((x >> 24) & 0xff))
#define OFFSET(base, ptr, member) (base + ((const char*)(&member) - (const char*)ptr)) 
#define SIZE(x) sizeof(x)




#define MAX_PAYLOAD_SIZE          500
#define PART_NUMBER_SIZE          16
#define DATE_SIZE                 6
#define HARD_RESERVED_SIZE1       3
#define HARD_RESERVED_SIZE2       2
#define JOBCARD_SIZE              12
#define SERIAL_NUMBER_SIZE        4
#define REPAIR_COUNT_SIZE         2
#define HARDWARE_REVISION_SIZE    6
#define OCP_SIZE                  20
#define SOFTWARE_REVISION_SIZE    8
#define USER_SIZE                 32
#define YEAR_SIZE                 2




enum LOADER_MESSAGE_IDS
{
  CMD_NONE,
  CMD_GET_HARDWARE_LABEL,
  RSP_HARDWARE_LABEL,
  CMD_GET_SOFTWARE_LABEL,
  RSP_SOFTWARE_LABEL,
  CMD_READ_MEMORY,
  RSP_READ_MEMORY,
  CMD_GET_TRANSFER_INFO,
  RSP_TRANSFER_INFO,
  CMD_ERASE_FLASH            = 0x30,
  CMD_PROGRAM_FLASH,
  CMD_BLANK_CHECK,
  CMD_SET_HARDWARE_LABEL     = 0x60,
  CMD_START_SESSION,
  CMD_VERIFY_CODE,
  CMD_POLL,
  RSP_FLASH_ACKNOWLEDGE      = 0xf0,
  RSP_ACKNOWLEDGE
};


enum ERROR_CODES
{
  E_OK,                   /* None - command OK/successful                                      */
  E_INVALID_COMMAND,      /* Invalid command                                                   */
  E_INVALID_LENGTH,       /* he message is the incorrect length                                */
  E_CRC,                  /* Invalid CRC or checksum verification                              */
  E_INVALID_PARAMETER,    /* Message content error                                             */
  E_NO_SUPPORT,           /* This version or application does not support the command          */
  E_WRONG_MODE,           /* The remote device is in a mode that does not support this message */
  E_HARDWARE_LABEL,       /* Hardware label is missing                                         */
  E_WRITE_PROTECT,        /* Protection failure                                                */
  E_PROGRAM_FAULT,        /* Flash programming operation failed                                */
  E_ERASE_ERROR,          /* Flash Erase operation failed                                      */
  E_BLANK_CHECK_FAULT,    /* Flash blank check failed                                          */
  E_FLASH_OVERLAY,        /* Could not overlay the data in flash                               */
  E_VERIFY_ERROR,         /* Data does not match                                               */
  E_FLASH_FAILURE,        /* Flash failed                                                      */
};


enum PROCESSORS
{
  PROCESSOR_1,
  PROCESSOR_2,
  PROCESSOR_3,
  PROCESSOR_MAX
};/*PROCESSORS*/

enum FLASH_DEVICES
{
  FLASH_DEVICE_OCP,
  FLASH_DEVICE_LOADER,
  FLASH_DEVICE_USER,
  FLASH_DEVICE_MAX
};/*FLASH_DEVICES*/

enum FLASH_SPACES
{
  FLASH_NONE,
  FLASH_OCP,
  FLASH_LOADER,
  FLASH_USER,
  FLASH_MAX
};/*FLASH_SPACES*/

enum EEPROM_SPACES
{
  EEPROM_MAIN,
  EEPROM_SECONDARY1,
  EEPROM_SECONDARY2,
  EEPROM_SECONDARY3,
  EEPROM_SECONDARY4,
  EEPROM_SECONDARY5,
  EEPROM_MAX
};/*EEPROM_SPACES*/


enum HW_LABEL_NODES
{
  HWL_DAC2_MAIN,
  HWL_DAC2_SECONDARY,
  HWL_DAC2_PSU,
  HWL_DAC2_DISPLAY,
  HWL_DAC2_MOTHER,
  HWL_FEC_MAIN,
  HWL_FEC_ANALYSIS,
  HWL_ETF_MAIN,
  HWL_MAX
};/*HW_LABEL_NODES*/



struct BCDDate                                                              /* Date method used in the software label        */
{
  unsigned char   nDay;                                                     /* Day of the month in BCD e.g. 0x19 = 19th      */
  unsigned char   nMonth;                                                   /* Month of the year in BCD e.g. 0x12 = December */
  unsigned char   nYear[YEAR_SIZE];                                         /* Year in BCD e.g. {0x20, 0x04} = 2004          */
};



struct HardwareLabel                                               /* Avitronics hardware label as per document EJ42122b05   */
{
  char         PartNumber[PART_NUMBER_SIZE];                       /* 1 - 16   The part number of the card in ASCII          */
  char         SerialNumber[SERIAL_NUMBER_SIZE];                   /* 17 - 20  The serial number of the card in ASCII        */
  char         ManufactureDate[DATE_SIZE];                         /* 21 - 26  The manufacturing date in ASCII format YYYYMM */
  char         HardwareRevision[HARDWARE_REVISION_SIZE];           /* 27 - 32  The hardware revision in the form V01.00      */
  char         HardwareModStatus[1];                               /* 33 - 33  The hardware modification status - single character */
  char         cReserved1[HARD_RESERVED_SIZE1];                    /* 34 - 36  Not used - padded with spaces                 */
  char         JobCardNumber[JOBCARD_SIZE];                        /* 37 - 48  The job card number in ASCII                  */
  char         cLastRepairedDate[DATE_SIZE];                       /* 49 - 54  The last repaired date in ASCII format YYYYMM */
  char         cSequentialRepairCount[REPAIR_COUNT_SIZE];          /* 55 - 56  The number of the repair in ASCII             */
  char         cWarrantyExpiryDate[DATE_SIZE];                     /* 57 - 62  The warranty end date in ASCII format YYYYMM  */
  char         cReserved2[HARD_RESERVED_SIZE2];                    /* 63 - 64  Not used - padded with spaces                 */
};

struct EEPROMStructure                                                      /* Layout of the 128 byte EEPROM found on each card                             */
{
  HardwareLabel  stHardwareLabel;                                           /* 1 - 64   Avitronics hardware label as per document EJ42122b05                */
  unsigned long          nLabelBaseAddress;                                 /* 65 - 68  Software label base address 0 = 0x100 below flash top               */
  unsigned long          nFlashMaxSize[PROCESSOR_MAX][FLASH_DEVICE_MAX];    /* 69 - 116 Maximum flash size for each of the flash devices for each processor */
  unsigned char           OCPUserArea[OCP_SIZE];                            /* 117 - 128  The balance of the EEPROM can be utilised by the user             */
};


struct SoftwareLabel                                                        /* Avitronics software label as per document EJ42122b05   */
{
  char    srPartNumber[PART_NUMBER_SIZE];                                   /* The part number of the software in ASCII  e.g. PartNumber MARU = '480307YY22002   ' */
  char    srVersionLabel[SOFTWARE_REVISION_SIZE];                           /* The software version in the form SV 01.00                                           */
  BCDDate stReleaseDate;                                                    /* Release date in packed BCD                                                          */
  BCDDate stCompileDate;                                                    /* Compile date in packed BCD                                                          */
  char    srUserData[USER_SIZE];                                            /* User data - usually ASCII indication or name of the software product                */
};


struct MsgLoaderHeader
{
  unsigned short          nCommand;
  unsigned short          nSequence;
  unsigned short          nLength;
  unsigned short          nFCS16;
};

struct MsgAcknowledge
{
  unsigned short          nCommand;
  unsigned short          nStatus;
  unsigned short          nParameter;
  unsigned short          nReserved;
};

struct MsgRequestLabel
{
  unsigned short          nDevice;
  unsigned short          nReserved;
};

struct MsgTransferRequest
{
  unsigned short          nType;
  unsigned short          nReserved;
};

struct MsgEEPROMLabel
{
  unsigned short           nDevice;
  unsigned short           nReserved;
  EEPROMStructure stHardwareRecord;
};

struct MsgSoftwareLabel
{
  unsigned short           nType;                                           /* The type of flash space from whence this label comes */
  unsigned short           nReserved;
  SoftwareLabel   stSoftwareRecord;
};

struct FlashHeader                                                 /* Used in every flash command and acknowledge                      */
{
  unsigned short           nType;                                           /* The type of flash being addressed (OCP, loader or auxiliary)     */
  unsigned short           nReserved;                                       /* Rounds the message up to a 32 bit boundary                       */
  unsigned long           nStartAddress;                                   /* The offset in bytes into the flash where the command must begin  */
  unsigned long           nLength;                                         /* The length in bytes that must be written or read                 */
};

typedef unsigned char  MessageContents[512 - sizeof(FlashHeader)];


struct MsgReadWriteMemory                                              /* Used to read from or write to flash memory                      */
{
  FlashHeader        stFlashHeader;                                 /* Memory type, address and length                                */
  MessageContents    stContents;                                    /* The contents of the bytes read or written to flash             */
};


struct MsgVerifyData                                                  /* Used to verify the integrity of the code loaded to the flash RAM */
{
  unsigned short           nType;                                           /* The type of flash being addressed (OCP, loader or auxiliary)     */
  unsigned short           nFCS16;                                          /* The FCS16 CRC of the data from nLowestData to nHighestData       */
  unsigned long           nLowestData;                                     /* The offset in bytes into the flash where the code or data starts */
  unsigned long           nHighestData;                                    /* The offset in bytes into the flash where the code or data ends   */
};

struct MsgFlashAcknowledge                                            /* Used to respond to flash commands Erase, blank check, program and read flash */
{
  MsgAcknowledge    stStandard;                                       /* The command and status that resulted                             */
  FlashHeader    stHeader;                                         /* The flash command parameters that resulted in this acknowledge   */
  unsigned long          nAddress;                                         /* The offset in bytes into the flash where the a problem occurred  */
  unsigned char           nRequiredValue;                                   /* The value required to be at the nAddress in the flash            */
  unsigned char           nActualValue;                                     /* The actual value found at the nAddress in the flash              */
  unsigned short          nReserved;                                        /* Rounds the message up to a 32 bit boundary                       */
};



#pragma pack(pop)



//-----------------------------------------------------------------------------
plp_EspLoader::plp_EspLoader(int layerIndex) :
    _layerIndex(layerIndex)
{

}


//-----------------------------------------------------------------------------
plp_EspLoader::~plp_EspLoader()
{

}


//-----------------------------------------------------------------------------
std::string plp_EspLoader::parseInfo(const unsigned char* data, size_t length, size_t& layerOffset, size_t& layerLength)
{
    // Cast the header
    struct MsgLoaderHeader* header = (struct MsgLoaderHeader*)(data + layerOffset);
    return std::string("ESP Loader Protocol: ") + commandToText(NTOHS(header->nCommand));
}


//-----------------------------------------------------------------------------
PacketLayerInfo* plp_EspLoader::parseDetail(const unsigned char* data, size_t length, size_t& layerOffset, size_t& layerLength)
{    

    // Cast the common header
    struct MsgLoaderHeader* header = (struct MsgLoaderHeader*)(data + layerOffset);

    // Create the info object
    PacketLayerInfo* packetLayerInfo = new PacketLayerInfo(
        layerOffset,
        layerLength,
        std::string("ESP2 Loader Protocol: ") + commandToText(NTOHS(header->nCommand)), 
        _layerIndex);

    // Add the fields
    packetLayerInfo->addFieldInfo(new PacketFieldInfo(packetLayerInfo, OFFSET(layerOffset, header, header->nCommand), SIZE(header->nCommand), "Command: 0x%04X", NTOHS(header->nCommand)));
    packetLayerInfo->addFieldInfo(new PacketFieldInfo(packetLayerInfo, OFFSET(layerOffset, header, header->nSequence), SIZE(header->nSequence), "Sequence: %d", NTOHS(header->nSequence)));
    packetLayerInfo->addFieldInfo(new PacketFieldInfo(packetLayerInfo, OFFSET(layerOffset, header, header->nLength), SIZE(header->nLength), "Length: %d", NTOHS(header->nLength)));
    packetLayerInfo->addFieldInfo(new PacketFieldInfo(packetLayerInfo, OFFSET(layerOffset, header, header->nFCS16), SIZE(header->nFCS16), "Checksum: %04X", NTOHS(header->nFCS16)));

    size_t subLayerOffset = layerOffset + sizeof(MsgLoaderHeader);
    size_t subLayerLength = layerLength - sizeof(MsgLoaderHeader);

    switch (NTOHS(header->nCommand)) {
        case CMD_GET_HARDWARE_LABEL: parseCmdGetHardwareLabel(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case RSP_HARDWARE_LABEL: parseRspHardwareLabel(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case CMD_GET_SOFTWARE_LABEL: parseCmdGetSoftwareLabel(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case RSP_SOFTWARE_LABEL: parseRspSoftwareLabel(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case CMD_READ_MEMORY: parseCmdReadMemory(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case RSP_READ_MEMORY: parseRspReadMemory(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case CMD_GET_TRANSFER_INFO: parseCmdGetTransferInfo(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case RSP_TRANSFER_INFO: parseRspTransferInfo(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case CMD_ERASE_FLASH: parseCmdEraseFlash(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case CMD_PROGRAM_FLASH: parseCmdProgramFlash(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case CMD_BLANK_CHECK: parseCmdBlankCheck(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case CMD_SET_HARDWARE_LABEL: parseCmdSetHardwareLabel(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case CMD_START_SESSION: parseCmdStartSession(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case CMD_VERIFY_CODE: parseCmdVerifyCode(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case CMD_POLL: parseCmdPoll(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case RSP_FLASH_ACKNOWLEDGE: parseRspFlashAcknowledge(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
        case RSP_ACKNOWLEDGE: parseRspAcknowledge(packetLayerInfo, data + subLayerOffset, subLayerLength, subLayerOffset); break;
    }

    // Return
    return packetLayerInfo;
}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseCmdGetHardwareLabel(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct MsgRequestLabel* msg = (struct MsgRequestLabel*)data;
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nDevice), SIZE(msg->nDevice), "Device: %d", NTOHS(msg->nDevice)));
}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseRspHardwareLabel(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct MsgEEPROMLabel* msg = (struct MsgEEPROMLabel*)data;

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nDevice), SIZE(msg->nDevice), "Device: %d", NTOHS(msg->nDevice)));
   
    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.PartNumber),
            SIZE(msg->stHardwareRecord.stHardwareLabel.PartNumber),
            std::string("Part Number: ") + std::string(msg->stHardwareRecord.stHardwareLabel.PartNumber, PART_NUMBER_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.SerialNumber),
            SIZE(msg->stHardwareRecord.stHardwareLabel.SerialNumber),
            std::string("Serial Number: ") + std::string(msg->stHardwareRecord.stHardwareLabel.SerialNumber, SERIAL_NUMBER_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.ManufactureDate),
            SIZE(msg->stHardwareRecord.stHardwareLabel.ManufactureDate),
            std::string("Manufacture Date: ") + std::string(msg->stHardwareRecord.stHardwareLabel.ManufactureDate, DATE_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.HardwareRevision),
            SIZE(msg->stHardwareRecord.stHardwareLabel.HardwareRevision),
            std::string("Hardware Revision: ") + std::string(msg->stHardwareRecord.stHardwareLabel.HardwareRevision, HARDWARE_REVISION_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.HardwareModStatus),
            SIZE(msg->stHardwareRecord.stHardwareLabel.HardwareModStatus),
            std::string("Hardware Mod Status: ") + std::string(msg->stHardwareRecord.stHardwareLabel.HardwareModStatus, 1)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.JobCardNumber),
            SIZE(msg->stHardwareRecord.stHardwareLabel.JobCardNumber),
            std::string("Jobcard Number: ") + std::string(msg->stHardwareRecord.stHardwareLabel.JobCardNumber, JOBCARD_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.cLastRepairedDate),
            SIZE(msg->stHardwareRecord.stHardwareLabel.cLastRepairedDate),
            std::string("Last Repaired Date: ") + std::string(msg->stHardwareRecord.stHardwareLabel.cLastRepairedDate, DATE_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.cSequentialRepairCount),
            SIZE(msg->stHardwareRecord.stHardwareLabel.cSequentialRepairCount),
            std::string("Sequential Repair Count: ") + std::string(msg->stHardwareRecord.stHardwareLabel.cSequentialRepairCount, REPAIR_COUNT_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.cWarrantyExpiryDate),
            SIZE(msg->stHardwareRecord.stHardwareLabel.cWarrantyExpiryDate),
            std::string("Warranty Expiry Date: ") + std::string(msg->stHardwareRecord.stHardwareLabel.cWarrantyExpiryDate, DATE_SIZE)));

}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseCmdGetSoftwareLabel(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct MsgTransferRequest* msg = (struct MsgTransferRequest*)data;
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nType), SIZE(msg->nType), "Flash Type: %s", flashTypeToText(NTOHS(msg->nType))));
}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseRspSoftwareLabel(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct MsgSoftwareLabel* msg = (struct MsgSoftwareLabel*)data;

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nType), SIZE(msg->nType), "Flash Type: %s", flashTypeToText(NTOHS(msg->nType))));
   
    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stSoftwareRecord.srPartNumber[0]),
            SIZE(msg->stSoftwareRecord.srPartNumber),
            std::string("Part Number: ") + std::string(msg->stSoftwareRecord.srPartNumber, PART_NUMBER_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stSoftwareRecord.srVersionLabel[0]),
            SIZE(msg->stSoftwareRecord.srVersionLabel),
            std::string("Version: ") + std::string(msg->stSoftwareRecord.srVersionLabel, SOFTWARE_REVISION_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stSoftwareRecord.stReleaseDate),
            SIZE(msg->stSoftwareRecord.stReleaseDate),
            "Release Date: %d%d%d%d-%d%d-%d%d",
            ((msg->stSoftwareRecord.stReleaseDate.nYear[0] & 0xF0) >> 4),
            (msg->stSoftwareRecord.stReleaseDate.nYear[0] & 0xF),
            ((msg->stSoftwareRecord.stReleaseDate.nYear[1] & 0xF0) >> 4),
            (msg->stSoftwareRecord.stReleaseDate.nYear[1] & 0xF),
            ((msg->stSoftwareRecord.stReleaseDate.nMonth & 0xF0) >> 4),
            (msg->stSoftwareRecord.stReleaseDate.nMonth & 0xF),
            ((msg->stSoftwareRecord.stReleaseDate.nDay & 0xF0) >> 4),
            (msg->stSoftwareRecord.stReleaseDate.nDay & 0xF)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stSoftwareRecord.stCompileDate),
            SIZE(msg->stSoftwareRecord.stCompileDate),
            "Compile Date: %d%d%d%d-%d%d-%d%d",
            ((msg->stSoftwareRecord.stCompileDate.nYear[0] & 0xF0) >> 4),
            (msg->stSoftwareRecord.stCompileDate.nYear[0] & 0xF),
            ((msg->stSoftwareRecord.stCompileDate.nYear[1] & 0xF0) >> 4),
            (msg->stSoftwareRecord.stCompileDate.nYear[1] & 0xF),
            ((msg->stSoftwareRecord.stCompileDate.nMonth & 0xF0) >> 4),
            (msg->stSoftwareRecord.stCompileDate.nMonth & 0xF),
            ((msg->stSoftwareRecord.stCompileDate.nDay & 0xF0) >> 4),
            (msg->stSoftwareRecord.stCompileDate.nDay & 0xF)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stSoftwareRecord.srUserData[0]),
            SIZE(msg->stSoftwareRecord.srUserData),
            std::string("User Data: ") + std::string(msg->stSoftwareRecord.srUserData, USER_SIZE)));
}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseCmdReadMemory(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct FlashHeader* header = (struct FlashHeader*)data;

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, data, header->nType), SIZE(header->nType), "Flash Type: %s", flashTypeToText(NTOHS(header->nType))));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, data, header->nStartAddress), SIZE(header->nStartAddress), "Address: 0x%08X", NTOHL(header->nStartAddress)));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, data, header->nLength), SIZE(header->nLength), "Length: %d", NTOHL(header->nLength)));
}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseRspReadMemory(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct FlashHeader* header = (struct FlashHeader*)data;

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, data, header->nType), SIZE(header->nType), "Flash Type: %s", flashTypeToText(NTOHS(header->nType))));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, data, header->nStartAddress), SIZE(header->nStartAddress), "Address: 0x%08X", NTOHL(header->nStartAddress)));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, data, header->nLength), SIZE(header->nLength), "Length: %d", NTOHL(header->nLength)));
    info->addFieldInfo(new PacketFieldInfo(info, offset + sizeof(FlashHeader), NTOHL(header->nLength), "Payload"));
}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseCmdGetTransferInfo(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{

}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseRspTransferInfo(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{

}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseCmdEraseFlash(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct FlashHeader* header = (struct FlashHeader*)data;

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, header, header->nType), SIZE(header->nType), "Flash Type: %s",flashTypeToText(NTOHS(header->nType))));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, header, header->nStartAddress), SIZE(header->nStartAddress), "Address: 0x%08X", NTOHL(header->nStartAddress)));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, header, header->nLength), SIZE(header->nLength), "Length: %d", NTOHL(header->nLength)));
}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseCmdProgramFlash(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct FlashHeader* header = (struct FlashHeader*)data;

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, data, header->nType), SIZE(header->nType), "Flash Type: %s", flashTypeToText(NTOHS(header->nType))));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, data, header->nStartAddress), SIZE(header->nStartAddress), "Address: 0x%08X", NTOHL(header->nStartAddress)));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, data, header->nLength), SIZE(header->nLength), "Length: %d", NTOHL(header->nLength)));
    info->addFieldInfo(new PacketFieldInfo(info, offset + sizeof(FlashHeader), NTOHL(header->nLength), "Payload"));
}

//-----------------------------------------------------------------------------
void plp_EspLoader::parseCmdBlankCheck(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct FlashHeader* header = (struct FlashHeader*)data;

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, header, header->nType), SIZE(header->nType), "Flash Type: %s", flashTypeToText(NTOHS(header->nType))));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, header, header->nStartAddress), SIZE(header->nStartAddress), "Address: 0x%08X", NTOHL(header->nStartAddress)));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, header, header->nLength), SIZE(header->nLength), "Length: %d", NTOHL(header->nLength)));
}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseCmdSetHardwareLabel(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct MsgEEPROMLabel* msg = (struct MsgEEPROMLabel*)data;

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nDevice), SIZE(msg->nDevice), "Device: %d", NTOHS(msg->nDevice)));
   
    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.PartNumber),
            SIZE(msg->stHardwareRecord.stHardwareLabel.PartNumber),
            std::string("Part Number: ") + std::string(msg->stHardwareRecord.stHardwareLabel.PartNumber, PART_NUMBER_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.SerialNumber),
            SIZE(msg->stHardwareRecord.stHardwareLabel.SerialNumber),
            std::string("Serial Number: ") + std::string(msg->stHardwareRecord.stHardwareLabel.SerialNumber, SERIAL_NUMBER_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.ManufactureDate),
            SIZE(msg->stHardwareRecord.stHardwareLabel.ManufactureDate),
            std::string("Manufacture Date: ") + std::string(msg->stHardwareRecord.stHardwareLabel.ManufactureDate, DATE_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.HardwareRevision),
            SIZE(msg->stHardwareRecord.stHardwareLabel.HardwareRevision),
            std::string("Hardware Revision: ") + std::string(msg->stHardwareRecord.stHardwareLabel.HardwareRevision, HARDWARE_REVISION_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.HardwareModStatus),
            SIZE(msg->stHardwareRecord.stHardwareLabel.HardwareModStatus),
            std::string("Hardware Mod Status: ") + std::string(msg->stHardwareRecord.stHardwareLabel.HardwareModStatus, 1)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.JobCardNumber),
            SIZE(msg->stHardwareRecord.stHardwareLabel.JobCardNumber),
            std::string("Jobcard Number: ") + std::string(msg->stHardwareRecord.stHardwareLabel.JobCardNumber, JOBCARD_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.cLastRepairedDate),
            SIZE(msg->stHardwareRecord.stHardwareLabel.cLastRepairedDate),
            std::string("Last Repaired Date: ") + std::string(msg->stHardwareRecord.stHardwareLabel.cLastRepairedDate, DATE_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.cSequentialRepairCount),
            SIZE(msg->stHardwareRecord.stHardwareLabel.cSequentialRepairCount),
            std::string("Sequential Repair Count: ") + std::string(msg->stHardwareRecord.stHardwareLabel.cSequentialRepairCount, REPAIR_COUNT_SIZE)));

    info->addFieldInfo(
        new PacketFieldInfo(
            info,
            OFFSET(offset, msg, msg->stHardwareRecord.stHardwareLabel.cWarrantyExpiryDate),
            SIZE(msg->stHardwareRecord.stHardwareLabel.cWarrantyExpiryDate),
            std::string("Warranty Expiry Date: ") + std::string(msg->stHardwareRecord.stHardwareLabel.cWarrantyExpiryDate, DATE_SIZE)));
}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseCmdStartSession(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{

}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseCmdVerifyCode(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct MsgVerifyData* msg = (struct MsgVerifyData*)data;

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nType), SIZE(msg->nType), "Flash Type: %s", flashTypeToText(NTOHS(msg->nType))));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nFCS16), SIZE(msg->nFCS16), "Checksum: 0x%04X", NTOHS(msg->nFCS16)));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nLowestData), SIZE(msg->nLowestData), "Lowest: 0x%08X", NTOHL(msg->nLowestData)));   
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nHighestData), SIZE(msg->nHighestData), "Highest: 0x%08X", NTOHL(msg->nHighestData)));   
}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseCmdPoll(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{

}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseRspFlashAcknowledge(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct MsgFlashAcknowledge* msg = (struct MsgFlashAcknowledge*)data;

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->stStandard.nCommand), SIZE(msg->stStandard.nCommand), "Ack Command: 0x%04X", NTOHS(msg->stStandard.nCommand)));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->stStandard.nStatus), SIZE(msg->stStandard.nStatus), "Ack Status: %s", errorCodeToText(NTOHS(msg->stStandard.nStatus))));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->stStandard.nParameter), SIZE(msg->stStandard.nParameter), "Ack Parameter: %d", NTOHS(msg->stStandard.nParameter)));   

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->stHeader.nType), SIZE(msg->stHeader.nType), "Flash Type: %s", flashTypeToText(NTOHS(msg->stHeader.nType))));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->stHeader.nStartAddress), SIZE(msg->stHeader.nStartAddress), "Address: 0x%08X", NTOHL(msg->stHeader.nStartAddress)));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->stHeader.nLength), SIZE(msg->stHeader.nLength), "Length: %d", NTOHL(msg->stHeader.nLength)));

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nAddress), SIZE(msg->nAddress), "Fault Address: 0x%08X", NTOHL(msg->nAddress)));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nRequiredValue), SIZE(msg->nRequiredValue), "Required Value: 0x%02X", msg->nRequiredValue));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nActualValue), SIZE(msg->nActualValue), "Actual Value: 0x%02X", msg->nActualValue));
}


//-----------------------------------------------------------------------------
void plp_EspLoader::parseRspAcknowledge(PacketLayerInfo* info, const unsigned char* data, size_t length, size_t offset)
{
    struct MsgAcknowledge* msg = (struct MsgAcknowledge*)data;

    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nCommand), SIZE(msg->nCommand), "Ack Command: 0x%04X", NTOHS(msg->nCommand)));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nStatus), SIZE(msg->nStatus), "Ack Status: %s", errorCodeToText(NTOHS(msg->nStatus))));
    info->addFieldInfo(new PacketFieldInfo(info, OFFSET(offset, msg, msg->nParameter), SIZE(msg->nParameter), "Ack Parameter: %d", NTOHS(msg->nParameter)));   
}


//-----------------------------------------------------------------------------
const char* plp_EspLoader::commandToText(unsigned short command) const
{
    switch (command) {
        case CMD_NONE:                  return "CMD_NONE";
        case CMD_GET_HARDWARE_LABEL:    return "CMD_GET_HARDWARE_LABEL";
        case RSP_HARDWARE_LABEL:        return "RSP_HARDWARE_LABEL";
        case CMD_GET_SOFTWARE_LABEL:    return "CMD_GET_SOFTWARE_LABEL";
        case RSP_SOFTWARE_LABEL:        return "RSP_SOFTWARE_LABEL";
        case CMD_READ_MEMORY:           return "CMD_READ_MEMORY";
        case RSP_READ_MEMORY:           return "RSP_READ_MEMORY";
        case CMD_GET_TRANSFER_INFO:     return "CMD_GET_TRANSFER_INFO";
        case RSP_TRANSFER_INFO:         return "RSP_TRANSFER_INFO";
        case CMD_ERASE_FLASH:           return "CMD_ERASE_FLASH";
        case CMD_PROGRAM_FLASH:         return "CMD_PROGRAM_FLASH";
        case CMD_BLANK_CHECK:           return "CMD_BLANK_CHECK";
        case CMD_SET_HARDWARE_LABEL:    return "CMD_SET_HARDWARE_LABEL";
        case CMD_START_SESSION:         return "CMD_START_SESSION";
        case CMD_VERIFY_CODE:           return "CMD_VERIFY_CODE";
        case CMD_POLL:                  return "CMD_POLL";
        case RSP_FLASH_ACKNOWLEDGE:     return "RSP_FLASH_ACKNOWLEDGE";
        case RSP_ACKNOWLEDGE:           return "RSP_ACKNOWLEDGE";
        default: return "UNKNOWN";
    }
}


//-----------------------------------------------------------------------------
const char* plp_EspLoader::errorCodeToText(unsigned short code) const
{
    switch (code) {
        case E_OK: return "E_OK";
        case E_INVALID_COMMAND: return "E_INVALID_COMMAND";
        case E_INVALID_LENGTH: return "E_INVALID_LENGTH";
        case E_CRC: return "E_CRC";
        case E_INVALID_PARAMETER: return "E_INVALID_PARAMETER";
        case E_NO_SUPPORT: return "E_NO_SUPPORT";
        case E_WRONG_MODE: return "E_WRONG_MODE";
        case E_HARDWARE_LABEL: return "E_HARDWARE_LABEL";
        case E_WRITE_PROTECT: return "E_WRITE_PROTECT";
        case E_PROGRAM_FAULT: return "E_PROGRAM_FAULT";
        case E_ERASE_ERROR: return "E_ERASE_ERROR";
        case E_BLANK_CHECK_FAULT: return "E_BLANK_CHECK_FAULT";
        case E_FLASH_OVERLAY: return "E_FLASH_OVERLAY";
        case E_VERIFY_ERROR: return "E_VERIFY_ERROR";
        case E_FLASH_FAILURE: return "E_FLASH_FAILURE";
        default: return "UNKNOWN";
    }
}


//-----------------------------------------------------------------------------
const char* plp_EspLoader::flashTypeToText(unsigned short type) const
{
    switch (type) {
        case FLASH_NONE: return "NONE";
        case FLASH_OCP: return "OCP";
        case FLASH_LOADER: return "LOADER";
        case FLASH_USER: return "USER";
        default: return "UNKNOWN";
    }
}

