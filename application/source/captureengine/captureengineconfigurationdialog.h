/*  captureengineconfiguration.h
 *
 *  Copyright 2012 Willem Kemp.
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
