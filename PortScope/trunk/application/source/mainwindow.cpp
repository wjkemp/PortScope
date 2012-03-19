#include "mainwindow.h"
#include <QDir>
#include <QPluginLoader>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <qmath.h>
#include "captureengine/captureengineconfigurationdialog.h"


//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags),
    _isConfigured(false),
    _isCapturing(false),
    _protocolStack(0),
    _captureEngine(0)
{

    setWindowTitle("PortScope");

    // Create MDI Area
    _mdiArea = new QMdiArea();
    setCentralWidget(_mdiArea);

    // Create the protocol stack
    _protocolStackView = new ProtocolStackView();

    _protocolStackDockWidget = new QDockWidget("Protocol Stack");
    _protocolStackDockWidget->setWidget(_protocolStackView);
    addDockWidget(Qt::BottomDockWidgetArea, _protocolStackDockWidget);


    // Create the actions
    _actOpenConfiguration = new QAction(QIcon(":/MainWindow/icons/open.png"), "Open Configuration", this);
    connect(_actOpenConfiguration, SIGNAL(triggered()), SLOT(openConfiguration()));

    _actCloseConfiguration = new QAction("Close Configuration", this);
    connect(_actCloseConfiguration, SIGNAL(triggered()), SLOT(closeConfiguration()));

    _actStartCapture = new QAction(QIcon(":/MainWindow/icons/startcapture.png"), "Start Capture", this);
    _actStartCapture->setEnabled(false);
    connect(_actStartCapture, SIGNAL(triggered()), SLOT(startCapture()));


    _actStopCapture = new QAction(QIcon(":/MainWindow/icons/stopcapture.png"), "Stop Capture", this);
    _actStopCapture->setEnabled(false);
    connect(_actStopCapture, SIGNAL(triggered()), SLOT(stopCapture()));

    _actTileSubWindows = new QAction("Tile", this);
    connect(_actTileSubWindows, SIGNAL(triggered()), _mdiArea, SLOT(tileSubWindows()));

    _actCascadeSubWindows = new QAction("Cascade", this);
    connect(_actCascadeSubWindows, SIGNAL(triggered()), _mdiArea, SLOT(cascadeSubWindows()));


    // Create the toolbar
    _toolBar = new QToolBar();
    _toolBar->addAction(_actOpenConfiguration);
    _toolBar->addSeparator();
    _toolBar->addAction(_actStartCapture);
    _toolBar->addAction(_actStopCapture);
    addToolBar(_toolBar);

    // Create the status bar
    _statusBar = new QStatusBar();
    _currentState = new QLabel();
    _currentState->setText("Idle");
    _statusBar->addPermanentWidget(_currentState);
    setStatusBar(_statusBar);


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

    QMenu* mWindow = new QMenu("Window");
    mWindow->addAction(_actTileSubWindows);
    mWindow->addAction(_actCascadeSubWindows);
    _menuBar->addMenu(mWindow);

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

    // Create a new protocol stack
    try {

        _protocolStack = new ProtocolStack("stack.xml");
        _captureEngine = new CaptureEngine(_protocolStack);
        _protocolStackView->setProtocolStack(_protocolStack);

        // Connect capture engine signals
        connect(_captureEngine, SIGNAL(started()), SLOT(captureStarted()));
        connect(_captureEngine, SIGNAL(stopped()), SLOT(captureStopped()));
        connect(_captureEngine, SIGNAL(error(const QString&)), SLOT(captureError(const QString&)));

        // Add display widgets to the widget stack
        QList<QWidget*> displayWidgets(_protocolStack->getDisplayWidgets());
        QWidget* displayWidget;
        foreach(displayWidget, displayWidgets) {
            QMdiSubWindow* subWindow = new QMdiSubWindow();
            subWindow->setStyleSheet("background: white;");
            subWindow->setWidget(displayWidget);
            _mdiArea->addSubWindow(subWindow);
            _mdiSubWindows[displayWidget] = subWindow;
            subWindow->show();
        }

        _mdiArea->tileSubWindows();

        connect(
            _protocolStackView,
            SIGNAL(displayWidgetChanged(QWidget*)),
            SLOT(showWidget(QWidget*)));

        _isConfigured = true;
        _actStartCapture->setEnabled(true);

    } catch (const std::exception& e) {

    }
}


//-----------------------------------------------------------------------------
void MainWindow::closeConfiguration()
{

    /* Delete all the MDI subwindows */
    QMapIterator<QWidget*, QMdiSubWindow*> i(_mdiSubWindows);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
    _mdiSubWindows.clear();

    /* Destroy the protocol stack */
    _protocolStackView->setProtocolStack(0);
    delete _protocolStack;
    delete _captureEngine;

    _protocolStack = 0;
    _captureEngine = 0;
}


//-----------------------------------------------------------------------------
void MainWindow::startCapture()
{
    if (_isConfigured && !_isCapturing) {

        CaptureEngineConfigurationDialog dialog;
        if (dialog.exec() == QDialog::Accepted) {
            _captureEngine->start(dialog.getConfiguration());
        }
    }

}


//-----------------------------------------------------------------------------
void MainWindow::stopCapture()
{
    if (_isConfigured && _isCapturing) {
        _captureEngine->stop();
    }
}


//-----------------------------------------------------------------------------
void MainWindow::showWidget(QWidget* widget)
{
    if (widget->isVisible()) {
    } else {
        widget->show();
    }
    _mdiArea->setActiveSubWindow(_mdiSubWindows[widget]);
}



//-----------------------------------------------------------------------------
void MainWindow::captureStarted()
{
    _isCapturing = true;
    _actStartCapture->setEnabled(false);
    _actStopCapture->setEnabled(true);
}


//-----------------------------------------------------------------------------
void MainWindow::captureStopped()
{
    _actStartCapture->setEnabled(true);
    _actStopCapture->setEnabled(false);
    _isCapturing = false;
}


//-----------------------------------------------------------------------------
void MainWindow::captureError(const QString& error)
{
    _isCapturing = false;
    QMessageBox::critical(this, "PortScope", error);
}

