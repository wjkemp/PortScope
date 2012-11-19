/*  mainwindow.h
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
#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QtGui/QMainWindow>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QSplitter>

#include "captureengine/captureengine.h"
#include "packetdatabase/parserconfiguration.h"
#include "packetdatabase/packetdatabase.h"
#include "views/packetlistmodel.h"
#include "views/packetlistview.h"
#include "views/packetlayermodel.h"
#include "views/packetlayerview.h"
#include "views/packetdataview.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~MainWindow();


protected:
    void closeEvent(QCloseEvent* event);

protected slots:
    void openConfiguration();
    void closeConfiguration();
    void startCapture();
    void stopCapture();
    void clearCapture();

    // Capture engine
    void captureStarted();
    void captureStopped();
    void captureError(const QString& error);

    void showAboutBox();

    void packetSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
    void packetFieldSelectionChanged(const QModelIndex& current, const QModelIndex& previous);

private:

    // Configuration-created objects
    bool _isConfigured;
    bool _isCapturing;
    bool _exitWhenCaptureEnds;
    CaptureEngine* _captureEngine;
    ParserConfiguration* _parserConfiguration;
    PacketDatabase* _packetDatabase;

    // Models and Views
    PacketListModel* _packetListModel;
    PacketListView* _packetListView;
    PacketLayerView* _packetLayerView;
    PacketDataView* _packetDataView;
    QSplitter* _splitter;


    // Menu and toolbars
    QMenuBar* _menuBar;
    QToolBar* _toolBar;
    QStatusBar* _statusBar;
    QLabel* _currentState;

    // Actions
    QAction* _actOpenConfiguration;
    QAction* _actCloseConfiguration;
    QAction* _actStartCapture;
    QAction* _actStopCapture;
    QAction* _actClearCapture;

    QAction* _actAbout;

};

#endif
