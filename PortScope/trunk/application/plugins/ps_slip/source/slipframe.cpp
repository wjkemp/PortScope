#include "slipframe.h"


//-----------------------------------------------------------------------------
SlipFrame::SlipFrame()
{

}


//-----------------------------------------------------------------------------
SlipFrame::SlipFrame(const SlipFrame& obj) :
    _timestamp(obj._timestamp),
    _frameLength(obj._frameLength),
    _payload(obj._payload)
{

}


//-----------------------------------------------------------------------------
SlipFrame::SlipFrame(const void* payload, size_t length)
{
    _timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    _frameLength = length;

    unsigned char* ptr = (unsigned char*)payload;
    _payload.reserve(length * 3);
    for (size_t i=0; i < length; ++i) {
        _payload += QString("%1").arg(ptr[i], 2, 16, QChar('0')).toUpper();
        _payload += " ";
    }

}


//-----------------------------------------------------------------------------
SlipFrame::~SlipFrame()
{

}
