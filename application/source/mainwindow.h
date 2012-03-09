#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QDockWidget>
#include <QStackedWidget>
#include <QMenuBar>
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

protected slots:
    void openConfiguration();
    void closeConfiguration();
    void startCapture();
    void stopCapture();

private:
  void loadPlugins();

private:

    // Static objects
    ProtocolStackView* _protocolStackView;
    QDockWidget* _protocolStackDockWidget;
    QStackedWidget* _displayWidgetStack;

    // Configuration-created objects
    bool _isConfigured;
    bool _isCapturing;
    ProtocolStack* _protocolStack;
    CaptureEngine* _captureEngine;


    // Menu and toolbars
    QMenuBar* _menuBar;

    // Actions
    QAction* _actOpenConfiguration;
    QAction* _actCloseConfiguration;
    QAction* _actStartCapture;
    QAction* _actStopCapture;
};

#endif // MAINWINDOW_H
