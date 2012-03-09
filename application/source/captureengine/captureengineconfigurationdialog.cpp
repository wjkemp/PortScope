#include "mainwindow.h"
#include <QDir>
#include <QPluginLoader>


//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
  ui.setupUi(this);
  loadPlugins();
}


//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{

}


//-----------------------------------------------------------------------------
void MainWindow::loadPlugins()
{

  QDir pluginsDir = QDir("../plugins/bin");

   foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
       QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
       QObject *plugin = loader.instance();
       if (plugin) {

       }
   }

}
