#ifndef __ROOTDISPATCHER_H__
#define __ROOTPLUGIN_H__

#include "protocolanalyzer.h"


class RootDispatcher : public ProtocolAnalyzer
{

public:

    ~RootDispatcher() {}
    QString name() const;
    QWidget* displayWidget();
    void addChild(ProtocolAnalyzer* child);
    void processData(const void* data, size_t length, Flags flags);

private:
    QList<ProtocolAnalyzer*> _children;
};


#endif
