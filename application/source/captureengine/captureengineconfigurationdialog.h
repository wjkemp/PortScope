#ifndef __CAPTUREENGINECONFIGURATIONDIALOG_H__
#define __CAPTUREENGINECONFIGURATIONDIALOG_H__

#include <QDialog>
#include "captureengineconfiguration.h"
#include "ui_captureengineconfigurationdialog.h"



class CaptureEngineConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    CaptureEngineConfigurationDialog(QWidget *parent = 0);
    ~CaptureEngineConfigurationDialog();

    CaptureEngineConfiguration getConfiguration() const { return _config; }

protected:
    void accept();

private:
    Ui::CaptureEngineConfigurationDialog ui;
    CaptureEngineConfiguration _config;
};

#endif
