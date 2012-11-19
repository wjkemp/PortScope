/*  captureengine.cpp
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
#include "captureengine.h"
#include <QMutexLocker>
#include <libps.h>
#include <QDateTime>


//-----------------------------------------------------------------------------
CaptureEngine::CaptureEngine(PacketDatabase* packetDatabase, PacketParser* packetParser, std::list<FramingProtocolParser*> framingProtocolParsers) :
    _packetParser(packetParser),
    _packetDatabase(packetDatabase),
    _framingProtocolParsers(framingProtocolParsers),
    _stop(false)
{
    connect(this, SIGNAL(packetReceived(Packet*)), _packetDatabase, SLOT(addPacket(Packet*)), Qt::QueuedConnection);
}


//-----------------------------------------------------------------------------
CaptureEngine::~CaptureEngine()
{

}


//-----------------------------------------------------------------------------
bool CaptureEngine::start(const CaptureEngineConfiguration& config)
{
    _config = config;

    QThread::start();
    return true;
}


//-----------------------------------------------------------------------------
void CaptureEngine::stop()
{
    QMutexLocker lock(&_lock);
    _stop = true;

}


//-----------------------------------------------------------------------------
void CaptureEngine::run()
{
    LIBPS_RESULT result;
    bool stop = false;


    // Create the capture buffer
    const size_t captureBufferSize = 2048;
    unsigned char* captureBuffer = new unsigned char[captureBufferSize];

    // Open the capture driver
    LIBPS_HANDLE device = LIBPS_Create(
        _config.portName().toStdWString().c_str(),
        captureBufferSize);

    if (device) {

        // Indicate start
        emit started();

        do {
            
            int flags = 0;
            result = LIBPS_WaitForData(device, &flags);            
            if (result == LIBPS_OK) {
                

                if (flags & LIBPS_TRANSMIT_DATA_AVAILABLE) {
                    size_t length = captureBufferSize;
                    result = LIBPS_ReadTransmitData(device, captureBuffer, &length);
                    process(captureBuffer, length, PF_TX);
                }

                if (flags & LIBPS_RECEIVE_DATA_AVAILABLE) {
                    size_t length = captureBufferSize;
                    result = LIBPS_ReadReceiveData(device, captureBuffer, &length);
                    process(captureBuffer, length, PF_RX);
                }
            }
            

            _lock.lock();
            stop = _stop;
            _lock.unlock();

        } while ((result == LIBPS_OK) && (stop == false));

        // Close the handle
        LIBPS_Close(device);

        // Delete the capture buffer
        delete [] captureBuffer;

        // Indicate stop
        _stop = false;
        emit stopped();

    } else {
        emit error("Could not open device");
    }

}


//-----------------------------------------------------------------------------
void CaptureEngine::process(const unsigned char* data, size_t length, PacketFlags flags)
{
    QDateTime time(QDateTime::currentDateTime());

    // Run through the list of framing protocol parsers
    std::list<FramingProtocolParser*>::const_iterator i;
    for (i = _framingProtocolParsers.begin(); i != _framingProtocolParsers.end(); ++i) {
        FramingProtocolParser* framingProtocolParser(*i);
        if (framingProtocolParser->supportedFlags() & flags) {
            std::list<Packet*> packets = framingProtocolParser->parse(data, length);
            std::list<Packet*>::const_iterator j;
            for (j = packets.begin(); j != packets.end(); ++j) {
                Packet* packet(*j);
                packet->setInfo(_packetParser->parseInfo(packet));
                packet->setTimestamp(time.toMSecsSinceEpoch());
                emit packetReceived(packet);
            }
        }
    }
}
