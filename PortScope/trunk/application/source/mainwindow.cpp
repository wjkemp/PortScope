#include "mainwindow.h"
#include <QDir>
#include <QPluginLoader>


//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags),
    _isConfigured(false),
    _isCapturing(false),
    _protocolStack(0),
    _captureEngine(0)
{

    // Create the widget stack
    _displayWidgetStack = new QStackedWidget();
    setCentralWidget(_displayWidgetStack);


    // Create the protocol stack
    _protocolStackView = new ProtocolStackView();

    _protocolStackDockWidget = new QDockWidget("Protocol Stack");
    _protocolStackDockWidget->setWidget(_protocolStackView);
    addDockWidget(Qt::LeftDockWidgetArea, _protocolStackDockWidget);


    // Create the actions
    _actOpenConfiguration = new QAction("Open Configuration", this);
    connect(_actOpenConfiguration, SIGNAL(triggered()), SLOT(openConfiguration()));

    _actCloseConfiguration = new QAction("Close Configuration", this);

    _actStartCapture = new QAction("Start Capture", this);
    connect(_actStartCapture, SIGNAL(triggered()), SLOT(startCapture()));


    _actStopCapture = new QAction("Stop Capture", this);


    // Create the menu bar
    _menuBar = new QMenuBar();
    QMenu* mFile = new QMenu("File");
    mFile->addAction(_actOpenConfiguration);
    mFile->addAction(_actCloseConfiguration);
    _menuBar->addMenu(mFile);

    QMenu* mCapture = new QMenu("Capture");
    mCapture->addAction(_actStartCapture);
    mCapture->addAction(_actStopCapture);
    _menuBar->addMenu(mCapture);
    setMenuBar(_menuBar);
}


//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{

}


//-----------------------------------------------------------------------------
void MainWindow::openConfiguration()
{

    // Close configuration if already open
    if (_isConfigured) {
        closeConfiguration();
    }

    // Create a new plugin stack
    try {

        // Create the plugin stack
        _protocolStack = new ProtocolStack("stack.xml");
        _captureEngine = new CaptureEngine(_protocolStack);
        _protocolStackView->setProtocolStack(_protocolStack);

        // Add display widgets to the widget stack
        QList<QWidget*> displayWidgets(_protocolStack->getDisplayWidgets());
        QWidget* displayWidget;
        foreach(displayWidget, displayWidgets) {
            _displayWidgetStack->addWidget(displayWidget);
        }

        connect(
            _protocolStackView,
            SIGNAL(displayWidgetChanged(QWidget*)),
            _displayWidgetStack, 
            SLOT(setCurrentWidget(QWidget*)));

        _isConfigured = true;

    } catch (const std::exception& e) {

    }
}


//-----------------------------------------------------------------------------
void MainWindow::closeConfiguration()
{

}


//-----------------------------------------------------------------------------
void MainWindow::startCapture()
{
    if (_isConfigured && !_isCapturing) {
        CaptureEngineConfiguration config;
        _captureEngine->start(config);
    }

}


//-----------------------------------------------------------------------------
void MainWindow::stopCapture()
{
    if (_isConfigured && _isCapturing) {
        _captureEngine->stop();
    }
}
