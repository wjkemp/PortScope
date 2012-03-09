#include "analyzer.h"


//-----------------------------------------------------------------------------
Analyzer::Analyzer() :
    SlipParser(1024)
{
    _displayModel = new DisplayModel();
    _displayView = new DisplayView();
    _displayView->setModel(_displayModel);
}


//-----------------------------------------------------------------------------
Analyzer::~Analyzer()
{

}


//-----------------------------------------------------------------------------
QWidget* Analyzer::displayWidget()
{
    return _displayView;
}


//-----------------------------------------------------------------------------
void Analyzer::addChild(ProtocolAnalyzer* child)
{
    _children.append(child);
}


//-----------------------------------------------------------------------------
void Analyzer::processData(const void* data, size_t length, Flags flags)
{
    process(data, length);
}


//-----------------------------------------------------------------------------
void Analyzer::frameReceived(const void* data, size_t length)
{
    // Log
    _displayModel->addItem(SlipFrame(data, length));

    // Forward
    ProtocolAnalyzer* child;
    foreach (child, _children) {
        child->processData(data, length, (Flags)0);
    }
}
