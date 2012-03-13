#include "captureengineconfigurationdialog.h"


//-----------------------------------------------------------------------------
CaptureEngineConfigurationDialog::CaptureEngineConfigurationDialog(QWidget *parent)
    : QDialog(parent)
{
  ui.setupUi(this);
}


//-----------------------------------------------------------------------------
CaptureEngineConfigurationDialog::~CaptureEngineConfigurationDialog()
{

}


//-----------------------------------------------------------------------------
void CaptureEngineConfigurationDialog::accept()
{
    _config.setPortName(ui.portName->text());
    QDialog::accept();
}
