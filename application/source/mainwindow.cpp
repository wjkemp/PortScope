#include "mainwindow.h"
#include <QDir>
#include <QPluginLoader>
#include <QMdiSubWindow>
#include <QMessageBox>
#include "captureengine/captureengineconfigurationdialog.h"


//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags),
    _isConfigured(false),
    _isCapturing(false),
    _protocolStack(0),
    _captureEngine(0)
{

    // Create MDI Area
    _mdiArea = new QMdiArea();
    setCentralWidget(_mdiArea);


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

    _actTileSubWindows = new QAction("Tile", this);
    connect(_actTileSubWindows, SIGNAL(triggered()), _mdiArea, SLOT(tileSubWindows()));

    _actCascadeSubWindows = new QAction("Cascade", this);
    connect(_actCascadeSubWindows, SIGNAL(triggered()), _mdiArea, SLOT(cascadeSubWindows()));



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

    // Create a new plugin stack
    try {

        // Create the plugin stack
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

}


//-----------------------------------------------------------------------------
void MainWindow::captureStopped()
{
    _isCapturing = false;

}


//-----------------------------------------------------------------------------
void MainWindow::captureError(const QString& error)
{
    _isCapturing = false;
    QMessageBox::critical(this, "PortScope", error);
}
