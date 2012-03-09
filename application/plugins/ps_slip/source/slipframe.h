#ifndef __SLIPFRAME_H__
#define __SLIPFRAME_H__

#include <QDateTime>


class SlipFrame
{

public:
    SlipFrame();
    SlipFrame(const SlipFrame& obj);
    SlipFrame(const void* payload, size_t length);
    ~SlipFrame();
    
    QString timestamp() const { return _timestamp; }
    size_t frameLength() const { return _frameLength; }
    QString payload() const { return _payload; }

private:
    QString _timestamp;
    size_t _frameLength;
    QString _payload;

};

#endif
