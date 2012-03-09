#ifndef __PROTOCOLANALYZER_H__
#define __PROTOCOLANALYZER_H__


#include <QWidget>


class ProtocolAnalyzer
{
public:
    enum Flags
    {
        TransmitData = 0x00000001,
        ReceiveData = 0x00000002
    };

public:
    virtual ~ProtocolAnalyzer() {}
    virtual void addChild(ProtocolAnalyzer* child) = 0;
    virtual QWidget* displayWidget() = 0;
    virtual void processData(const void* data, size_t length, Flags flags) = 0;
};



#endif