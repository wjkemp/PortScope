#include "captureengine.h"
#include "capturedevice.h"


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
    CaptureDevice* device = new CaptureDevice("\\\\.\\PortScope");
    //CaptureDevice* device = new CaptureDevice("\\\\.\\COM14");
    unsigned char* captureBuffer = new unsigned char[captureBufferSize];


    do {


        // Read from the capture interface
        size_t bytesCaptured = device->captureData(captureBuffer, captureBufferSize);

        // Process
        if (bytesCaptured) {
            _protocolStack->root()->analyzer()->processData(captureBuffer, bytesCaptured, ProtocolAnalyzer::TransmitData);
        }

        usleep(10);


    } while (1);

}

