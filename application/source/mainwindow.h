#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QDockWidget>
#include <QMdiArea>
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
    void showWidget(QWidget* widget);

    // Capture engine
    void captureStarted();
    void captureStopped();
    void captureError(const QString& error);

private:
  void loadPlugins();

private:

    // Static objects
    ProtocolStackView* _protocolStackView;
    QDockWidget* _protocolStackDockWidget;
    QMdiArea* _mdiArea;
    QMap<QWidget*, QMdiSubWindow*> _mdiSubWindows;

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

    QAction* _actTileSubWindows;
    QAction* _actCascadeSubWindows;

};

#endif // MAINWINDOW_H
