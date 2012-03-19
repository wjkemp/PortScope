#ifndef __ANALYZER_H__
#define __ANALYZER_H__

#include <protocolanalyzer.h>
#include "slipparser.h"
#include "displaymodel.h"
#include "displayview.h"
#include <QList>


class Analyzer : public QObject, public ProtocolAnalyzer, public SlipParser
{
    Q_OBJECT

public:
    Analyzer(const QString& name);
    ~Analyzer();
    QWidget* displayWidget();
    void addChild(ProtocolAnalyzer* child);
    void processData(const void* data, size_t length, Flags flags);

signals:
    void updateDisplay();

protected:
    void frameReceived(const void* data, size_t length);

private:
    QString _name;
    QList<ProtocolAnalyzer*> _children;
    DisplayModel* _displayModel;
    DisplayView* _displayView;
};

#endif
