#ifndef __ANALYZER_H__
#define __ANALYZER_H__

#include <protocolanalyzer.h>
#include "slipparser.h"
#include "displaymodel.h"
#include "displayview.h"
#include <QList>


class Analyzer : public ProtocolAnalyzer, public SlipParser
{

public:
    Analyzer();
    ~Analyzer();
    QWidget* displayWidget();
    void addChild(ProtocolAnalyzer* child);
    void processData(const void* data, size_t length, Flags flags);

protected:
    void frameReceived(const void* data, size_t length);

private:
    QList<ProtocolAnalyzer*> _children;
    DisplayModel* _displayModel;
    DisplayView* _displayView;
};

#endif
