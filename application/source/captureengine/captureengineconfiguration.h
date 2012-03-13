#ifndef __CAPTUREENGINECONFIGURATION_H__
#define __CAPTUREENGINECONFIGURATION_H__

#include <QString>


class CaptureEngineConfiguration
{

public:
    CaptureEngineConfiguration();
    ~CaptureEngineConfiguration();

    QString portName() const { return _portName; }

    void setPortName(const QString& portName) { _portName = portName; }

private:
    QString _portName;
};

#endif
