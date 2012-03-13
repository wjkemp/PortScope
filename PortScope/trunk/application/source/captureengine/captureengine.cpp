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
    _config = config;

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
    LIBPS_RESULT result;


    // Create the capture buffer
    const size_t captureBufferSize = 2048;
    unsigned char* captureBuffer = new unsigned char[captureBufferSize];

    // Open the capture driver
    LIBPS_HANDLE device = LIBPS_Create(
        _config.portName().toStdWString().c_str(),
        captureBufferSize);

    if (device) {

        do {

            int flags = 0;
            result = LIBPS_WaitForData(device, &flags);
            if (result == LIBPS_OK) {

                if (flags & LIBPS_TRANSMIT_DATA_AVAILABLE) {
                    size_t length = captureBufferSize;
                    result = LIBPS_ReadTransmitData(device, captureBuffer, &length);
                    _protocolStack->root()->analyzer()->processData(captureBuffer, length, ProtocolAnalyzer::TransmitData);
                }

                if (flags & LIBPS_RECEIVE_DATA_AVAILABLE) {
                    size_t length = captureBufferSize;
                    result = LIBPS_ReadReceiveData(device, captureBuffer, &length);
                    _protocolStack->root()->analyzer()->processData(captureBuffer, length, ProtocolAnalyzer::ReceiveData);
                }
            }

        } while (result == LIBPS_OK);

        // Close the handle
        LIBPS_Close(device);

    } else {
        emit error("Could not open device");
    }

}

