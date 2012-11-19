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
#include "packetdatabase/parserconfigurationloader.h"


//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags),
    _isConfigured(false),
    _isCapturing(false),
    _exitWhenCaptureEnds(false),
    _captureEngine(0),
    _parserConfiguration(0),
    _packetDatabase(0)
{

    setWindowTitle(
        QString("PortScope [Portscope ver %1.%2.%3]")
            .arg(PORTSCOPE_VERSION_MAJOR)
            .arg(PORTSCOPE_VERSION_MINOR)
            .arg(PORTSCOPE_VERSION_BUILD));



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

    _actClearCapture = new QAction(QIcon(":/MainWindow/icons/clear.png"), "Clear Capture", this);
    _actClearCapture->setEnabled(true);
    connect(_actClearCapture, SIGNAL(triggered()), SLOT(clearCapture()));


    _actAbout = new QAction("About", this);
    connect(_actAbout, SIGNAL(triggered()), SLOT(showAboutBox()));


    // Create the toolbar
    _toolBar = new QToolBar();
    _toolBar->addAction(_actOpenConfiguration);
    _toolBar->addSeparator();
    _toolBar->addAction(_actStartCapture);
    _toolBar->addAction(_actStopCapture);
    _toolBar->addAction(_actClearCapture);
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
    mCapture->addAction(_actClearCapture);
    _menuBar->addMenu(mCapture);

    QMenu* mHelp = new QMenu("Help");
    mHelp->addAction(_actAbout);
    _menuBar->addMenu(mHelp);

    setMenuBar(_menuBar);



    // Create the main widgets
    _packetDatabase = new PacketDatabase();
    _packetListModel = new PacketListModel(_packetDatabase);
    _packetListView = new PacketListView();
    _packetListView->setModel(_packetListModel);
    _packetLayerView = new PacketLayerView();
    _packetDataView = new PacketDataView();


    _packetDatabase->addListener(_packetListView);


    // Create the splitter
    _splitter = new QSplitter(Qt::Vertical);
    _splitter->setHandleWidth(3);
    _splitter->addWidget(_packetListView);
    _splitter->addWidget(_packetLayerView);
    _splitter->addWidget(_packetDataView);

    setCentralWidget(_splitter);


    connect(_packetListView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), SLOT(packetSelectionChanged(const QModelIndex&, const QModelIndex&)));

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

            ParserConfigurationLoader loader;
            _parserConfiguration = loader.load(fileName);
            _captureEngine = new CaptureEngine(_packetDatabase, _parserConfiguration->packetParser(), _parserConfiguration->framingProtocolParsers());

            // Connect capture engine signals
            connect(_captureEngine, SIGNAL(started()), SLOT(captureStarted()), Qt::QueuedConnection);
            connect(_captureEngine, SIGNAL(stopped()), SLOT(captureStopped()), Qt::QueuedConnection);
            connect(_captureEngine, SIGNAL(error(const QString&)), SLOT(captureError(const QString&)), Qt::QueuedConnection);

            _isConfigured = true;
            _actStartCapture->setEnabled(true);

        } catch (const std::exception& e) {

        }
    }
}


//-----------------------------------------------------------------------------
void MainWindow::closeConfiguration()
{

    // Destroy the protocol stack
    delete _parserConfiguration;
    delete _captureEngine;
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
void MainWindow::clearCapture()
{
    _packetDatabase->clear();
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
void MainWindow::packetSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if (current.isValid()) {
        Packet* packet = _packetDatabase->packetAt(current.row());
        if (packet) {

            // Parse the packet if it is not parsed
            if (!packet->parsed()) {
                PacketParser* parser = _parserConfiguration->packetParser();
                parser->parseDetail(packet);
            }

            // Set the new model
            PacketLayerModel* model = new PacketLayerModel(packet);
            _packetLayerView->setModel(model);
            connect(
                _packetLayerView->selectionModel(),
                SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
                SLOT(packetFieldSelectionChanged(const QModelIndex&, const QModelIndex&)));


            // Set the data view
            _packetDataView->clear();
            _packetDataView->addData(packet->data(), packet->length());
            _packetDataView->setHighlightField(0, 0);
            _packetDataView->update();
        }

    } else {
        _packetLayerView->setModel(0);
        _packetDataView->clear();
        _packetDataView->setHighlightField(0, 0);
        _packetDataView->update();        
    }
}


//-----------------------------------------------------------------------------
void MainWindow::packetFieldSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
    PacketLayerModel* model = static_cast<PacketLayerModel*>(_packetLayerView->model());
    if (model) {
        PacketInfo* info = model->getInfoAt(current);
        if (info) {
            _packetDataView->setHighlightField(info->offset(), info->length());
        }
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
