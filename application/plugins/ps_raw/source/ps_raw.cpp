#include "ps_raw.h"
#include "analyzer.h"


//-----------------------------------------------------------------------------
PS_raw::PS_raw()
{
}


//-----------------------------------------------------------------------------
PS_raw::~PS_raw()
{

}


//-----------------------------------------------------------------------------
QString PS_raw::displayText() const
{
    return "Filter";
}

//-----------------------------------------------------------------------------
ProtocolAnalyzer* PS_raw::createProtocolAnalyzer(const QString& name, const QString& parameters)
{
    return new Analyzer(name);
}


//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(ps_raw, PS_raw)
