#include "ps_slip.h"
#include "analyzer.h"


//-----------------------------------------------------------------------------
PS_slip::PS_slip()
{
}


//-----------------------------------------------------------------------------
PS_slip::~PS_slip()
{

}


//-----------------------------------------------------------------------------
QString PS_slip::displayText() const
{
    return "SLIP Framing Protocol";
}

//-----------------------------------------------------------------------------
ProtocolAnalyzer* PS_slip::createProtocolAnalyzer(const QString& name, const QString& parameters)
{
    return new Analyzer(name);
}


//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(ps_slip, PS_slip)
