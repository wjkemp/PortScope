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
ProtocolAnalyzer* PS_filter::createProtocolAnalyzer()
{
    return new Analyzer();
}


//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(ps_filter, PS_filter)
