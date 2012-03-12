#include "captureengine.h"
#include <libps.h>


//-----------------------------------------------------------------------------
CaptureEngine::CaptureEngine(ProtocolStack* protocolStack) :
    _protocolStack(protocolStack)
{

}


//-----------------------------------------------------------------------------
CaptureEngine::~CaptureEngine()
{

}


//-----------------------------------------------------------------------------
bool CaptureEngine::start(const CaptureEngineConfiguration& config)
{

    QThread::start();
    return true;
}


//-----------------------------------------------------------------------------
void CaptureEngine::stop()
{

}


//-----------------------------------------------------------------------------
void CaptureEngine::run()
{

    // Create the capture interface
    const size_t captureBufferSize = 2048;
    LIBPS_HANDLE device = LIBPS_Initialize(captureBufferSize);
    unsigned char* captureBuffer = new unsigned char[captureBufferSize];


    do {

        int flags = 0;
        LIBPS_WaitForData(device, &flags);

        if (flags & LIBPS_TRANSMIT_DATA_AVAILABLE) {
            size_t length = captureBufferSize;
            LIBPS_ReadTransmitData(device, captureBuffer, &length);
            _protocolStack->root()->analyzer()->processData(captureBuffer, length, ProtocolAnalyzer::TransmitData);
        }

        if (flags & LIBPS_RECEIVE_DATA_AVAILABLE) {
            size_t length = captureBufferSize;
            LIBPS_ReadReceiveData(device, captureBuffer, &length);
            _protocolStack->root()->analyzer()->processData(captureBuffer, length, ProtocolAnalyzer::ReceiveData);
        }

    } while (1);

}

