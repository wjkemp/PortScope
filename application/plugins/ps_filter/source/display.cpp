#include "display.h"
#include "analyzer.h"
#include <QGridLayout>



//-----------------------------------------------------------------------------
Display::Display(Analyzer* analyzer) :
    _analyzer(analyzer)
{
    QGridLayout* layout = new QGridLayout();

    _bytesProcessed = new QLabel("0");

    layout->addWidget(new QLabel("Bytes Processed: "), 0, 0);
    layout->addWidget(_bytesProcessed, 0, 1);
    layout->addItem(new QSpacerItem(0,0, QSizePolicy::Minimum, QSizePolicy::Expanding), 1, 0);
    layout->addItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2);

    setLayout(layout);



    _updateTimer = new QTimer();
    _updateTimer->setInterval(500);
    connect(_updateTimer, SIGNAL(timeout()), SLOT(onUpdate()));
    _updateTimer->start();

    setStyleSheet("background: white;");

}


//-----------------------------------------------------------------------------
Display::~Display()
{

}


//-----------------------------------------------------------------------------
void Display::onUpdate()
{
    _bytesProcessed->setText(QString::number(_analyzer->bytesProcessed()));
}
