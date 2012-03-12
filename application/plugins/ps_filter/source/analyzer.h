#ifndef __ANALYZER_H__
#define __ANALYZER_H__

#include <protocolanalyzer.h>
#include <QList>

class Display;


class Analyzer : public ProtocolAnalyzer
{

public:
    Analyzer(const QString& name, Flags filterMask);
    ~Analyzer();
    QWidget* displayWidget();
    void addChild(ProtocolAnalyzer* child);
    void processData(const void* data, size_t length, Flags flags);

    unsigned long bytesProcessed() const { return _bytesProcessed; }

private:
    QString _name;
    Flags _filterMask;
    QList<ProtocolAnalyzer*> _children;
    Display* _display;
    unsigned long _bytesProcessed;

};

#endif
