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
#include <QMdiArea>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include "protocolstack/protocolstack.h"
#include "protocolstack/protocolstackview.h"
#include "protocolstack/protocolstackmodel.h"

#include "captureengine/captureengine.h"


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
    void showWidget(QWidget* widget);

    // Capture engine
    void captureStarted();
    void captureStopped();
    void captureError(const QString& error);

    void showAboutBox();

private:

    // Static objects
    ProtocolStackView* _protocolStackView;
    QDockWidget* _protocolStackDockWidget;
    QMdiArea* _mdiArea;
    QMap<QWidget*, QMdiSubWindow*> _mdiSubWindows;

    // Configuration-created objects
    bool _isConfigured;
    bool _isCapturing;
    bool _exitWhenCaptureEnds;
    ProtocolStack* _protocolStack;
    CaptureEngine* _captureEngine;


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

    QAction* _actTileSubWindows;
    QAction* _actCascadeSubWindows;

    QAction* _actAbout;

};

#endif
