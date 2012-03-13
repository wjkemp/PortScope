#ifndef __CAPTUREENGINE_H__
#define __CAPTUREENGINE_H__

#include <QThread>
#include "protocolstack/protocolstack.h"
#include "captureengineconfiguration.h"


class CaptureEngine : public QThread
{
    Q_OBJECT

public:
    CaptureEngine(ProtocolStack* protocolStack);
    ~CaptureEngine();

    bool start(const CaptureEngineConfiguration& config);
    void stop();


signals:
    void started();
    void stopped();
    void error(const QString& error);


protected:
    void run();

private:
    ProtocolStack* _protocolStack;
    CaptureEngineConfiguration _config;

};

#endif
