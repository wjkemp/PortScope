#include "captureengine.h"
#include <QMutexLocker>
#include <libps.h>


//-----------------------------------------------------------------------------
CaptureEngine::CaptureEngine(ProtocolStack* protocolStack) :
    _protocolStack(protocolStack),
    _stop(false)
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
                    _protocolStack->root()->analyzer()->processData(captureBuffer, length, ProtocolAnalyzer::TransmitData);
                }

                if (flags & LIBPS_RECEIVE_DATA_AVAILABLE) {
                    size_t length = captureBufferSize;
                    result = LIBPS_ReadReceiveData(device, captureBuffer, &length);
                    _protocolStack->root()->analyzer()->processData(captureBuffer, length, ProtocolAnalyzer::ReceiveData);
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

