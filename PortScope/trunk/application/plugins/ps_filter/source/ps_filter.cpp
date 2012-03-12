#include "ps_filter.h"
#include "analyzer.h"


//-----------------------------------------------------------------------------
PS_filter::PS_filter()
{
}


//-----------------------------------------------------------------------------
PS_filter::~PS_filter()
{

}


//-----------------------------------------------------------------------------
QString PS_filter::displayText() const
{
    return "Filter";
}

//-----------------------------------------------------------------------------
ProtocolAnalyzer* PS_filter::createProtocolAnalyzer(const QString& name, const QString& parameters)
{
    if (parameters == "TX") {
        return new Analyzer(name, ProtocolAnalyzer::TransmitData);
    } else {
        return new Analyzer(name, ProtocolAnalyzer::ReceiveData);
    }
}


//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(ps_filter, PS_filter)
