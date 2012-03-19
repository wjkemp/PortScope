#ifndef __ANALYZER_H__
#define __ANALYZER_H__

#include <protocolanalyzer.h>
#include <QList>

class Display;


class Analyzer : public QObject, public ProtocolAnalyzer
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

private:
    QString _name;
    QList<ProtocolAnalyzer*> _children;
    Display* _display;
    size_t _bytesProcessed;

};

#endif
