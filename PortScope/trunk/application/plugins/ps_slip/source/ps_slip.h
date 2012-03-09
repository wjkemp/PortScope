#ifndef __PS_SLIP_H__
#define __PS_SLIP_H__

#include <QObject>
#include <QTextEdit>
#include <plugin.h>


class PS_slip : public QObject, public Plugin
{
  Q_OBJECT
  Q_INTERFACES(Plugin)

public:
    PS_slip();
    ~PS_slip();
    QString displayText() const;
    ProtocolAnalyzer* createProtocolAnalyzer();

};

#endif
