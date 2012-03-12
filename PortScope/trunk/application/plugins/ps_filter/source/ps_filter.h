#ifndef __PS_FILTER_H__
#define __PS_FILTER_H__

#include <QObject>
#include <QTextEdit>
#include <plugin.h>


class PS_filter : public QObject, public Plugin
{
  Q_OBJECT
  Q_INTERFACES(Plugin)

public:
    PS_filter();
    ~PS_filter();
    QString displayText() const;
    ProtocolAnalyzer* createProtocolAnalyzer(const QString& name, const QString& parameters);

};

#endif
