#ifndef __PSPLUGIN_H__
#define __PSPLUGIN_H__

#include <QtPlugin>
#include "protocolanalyzer.h"


class Plugin
{

public:
    virtual ~Plugin() {}
    virtual QString displayText() const = 0;
    virtual ProtocolAnalyzer* createProtocolAnalyzer() = 0;

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Plugin, "com.hurr.PortScope.Plugin/1.0")
QT_END_NAMESPACE


#endif