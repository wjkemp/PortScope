#ifndef __PS_RAW_H__
#define __PS_RAW_H__

#include <QObject>
#include <plugin.h>


class PS_raw : public QObject, public Plugin
{
  Q_OBJECT
  Q_INTERFACES(Plugin)

public:
    PS_raw();
    ~PS_raw();
    QString displayText() const;
    ProtocolAnalyzer* createProtocolAnalyzer(const QString& name, const QString& parameters);

};

#endif
