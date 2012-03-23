/*  mainwindow.cpp
 *
 *  Copyright (C) 2012 Willem Kemp <http://www.thenocturnaltree.com>
 *  All rights reserved.
 *
 *  This file is part of PortScope.
 *
 *  PortScope is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PortScope is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PortScope. If not, see http://www.gnu.org/licenses/.
 *
 */
#include "mainwindow.h"
#include "version.h"
#include <QDir>
#include <QPluginLoader>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include "captureengine/captureengineconfigurationdialog.h"


//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags),
    _isConfigured(false),
    _isCapturing(false),
    _exitWhenCaptureEnds(false),
    _protocolStack(0),
    _captureEngine(0)
{

    setWindowTitle(
        QString("PortScope [Portscope ver %1.%2.%3]")
            .arg(PORTSCOPE_VERSION_MAJOR)
            .arg(PORTSCOPE_VERSION_MINOR)
            .arg(PORTSCOPE_VERSION_BUILD));


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

    _actAbout = new QAction("About", this);
    connect(_actAbout, SIGNAL(triggered()), SLOT(showAboutBox()));


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

    QMenu* mHelp = new QMenu("Help");
    mHelp->addAction(_actAbout);
    _menuBar->addMenu(mHelp);

    setMenuBar(_menuBar);
}


//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{

}


//-----------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent* event)
{
    if (_isCapturing) {
        _exitWhenCaptureEnds = true;
        stopCapture();
        event->ignore();

    } else {
        event->accept();
    }
}


//-----------------------------------------------------------------------------
void MainWindow::openConfiguration()
{

    // Show the Open File Dialog
    QString fileName = QFileDialog::getOpenFileName(this, "Open Capture Configuration", QString(), "*.xml");
    if (!fileName.isEmpty()) {

        // Close configuration if already open
        if (_isConfigured) {
            closeConfiguration();
        }

        // Create a new protocol stack
        try {

            _protocolStack = new ProtocolStack(fileName);
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
            _actStartCapture->setEnabled(true);

        } catch (const std::exception& e) {

        }
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
            _currentState->setText(QString("Capturing on %1").arg(dialog.getConfiguration().portName()));
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
    _currentState->setText("Idle");

    if (_exitWhenCaptureEnds) {
        close();
    }
}


//-----------------------------------------------------------------------------
void MainWindow::captureError(const QString& error)
{
    _isCapturing = false;
    QMessageBox::critical(this, "PortScope", error);

    if (_exitWhenCaptureEnds) {
        close();
    }
}



//-----------------------------------------------------------------------------
void MainWindow::showAboutBox()
{
    QMessageBox::about(this, "About PortScope",
        "PortScope version " PORTSCOPE_VERSION_STRING "\n"
        "Copyright (C) 2012 Willem Kemp <http://www.thenocturnaltree.com>\n"
        "All rights reserved.\n\n"

        "PortScope is free software: you can redistribute it and/or modify\n"
        "it under the terms of the GNU General Public License as published by\n"
        "the Free Software Foundation, either version 3 of the License, or\n"
        "(at your option) any later version.\n\n"

        "PortScope is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
        "GNU General Public License for more details.\n\n"

        "You should have received a copy of the GNU General Public License\n"
        "along with PortScope. If not, see http://www.gnu.org/licenses/.");
}
