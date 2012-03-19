#include "analyzer.h"


//-----------------------------------------------------------------------------
Analyzer::Analyzer(const QString& name) :
    SlipParser(1024),
    _name(name)
{
    _displayModel = new DisplayModel();
    _displayView = new DisplayView();
    _displayView->setModel(_displayModel);
    _displayView->setWindowTitle(name);
    connect(this, SIGNAL(updateDisplay()), _displayView, SLOT(updateDisplay()));
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

    emit updateDisplay();

    // Forward
    ProtocolAnalyzer* child;
    foreach (child, _children) {
        child->processData(data, length, (Flags)0);
    }
}
