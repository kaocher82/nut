//
// C++ Implementation: deviceoptions
//
// Description: 
//
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QHeaderView>
#include <QInputDialog>
#include <QProcess>
#include "deviceoptions.h"
#include "deviceoptionsmodel.h"
#include "interfacedetailsmodel.h"
//#include "environmentdetailsmodel.h"
#include "ipconfiguration.h"
#include "scriptsettings.h"
#include "common.h"

namespace qnut {
	CDeviceOptions::CDeviceOptions(CDevice * parentDevice, QWidget * parent) :
		QWidget(parent),
		settings(UI_PATH_DEV(parentDevice->name) + "dev.conf", QSettings::IniFormat, this)
	{
		device = parentDevice;
		
		createView();
		createActions();
		
		trayIcon->setToolTip(shortSummary(device));
		trayIcon->setContextMenu(deviceMenu);
		connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(uiShowThisTab()));
		connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(uiHandleTrayActivated(QSystemTrayIcon::ActivationReason)));
		
		readSettings();
		
		connect(device, SIGNAL(stateChanged(DeviceState)), this, SLOT(uiHandleStateChange(DeviceState)));
		
		if (device->state == DS_UP)
			ui.environmentTree->expand(ui.environmentTree->model()->index(device->environments.indexOf(device->activeEnvironment), 0));
	}
	
	CDeviceOptions::~CDeviceOptions() {
		disconnect(device, SIGNAL(stateChanged(DeviceState)), this, SLOT(uiHandleStateChange(DeviceState)));
		disconnect(device, SIGNAL(environmentsUpdated()), ui.environmentTree, SLOT(reset()));
		writeSettings();
		delete deviceMenu;
	}
	
	inline void CDeviceOptions::readSettings() {
		settings.beginGroup("Main");
		scriptFlags = settings.value("scriptFlags", 0).toInt();
		trayIcon->setVisible(settings.value("showTrayIcon", false).toBool());
		ui.detailsButton->setChecked(settings.value("showDetails", false).toBool());
		settings.endGroup();
		ui.showTrayCheck->setChecked(trayIcon->isVisible());
	}
	
	inline void CDeviceOptions::writeSettings() {
		settings.beginGroup("Main");
		settings.setValue("scriptFlags", scriptFlags);
		settings.setValue("showTrayIcon", trayIcon->isVisible());
		settings.setValue("showDetails", ui.detailsButton->isChecked());
		settings.endGroup();
	}
	
	inline void CDeviceOptions::createActions() {
		deviceMenu = new QMenu(device->name, NULL);
		enableDeviceAction    = deviceMenu->addAction(QIcon(UI_ICON_DEVICE_ENABLE) , tr("Enable device") , device, SLOT(enable()));
		disableDeviceAction   = deviceMenu->addAction(QIcon(UI_ICON_DEVICE_DISABLE), tr("Disable device"), device, SLOT(disable()));
		deviceMenu->addSeparator();
		showAction            = deviceMenu->addAction(QIcon(UI_ICON_ENVIRONMENT), tr("Environments..."), this, SLOT(uiShowThisTab()));
		deviceSettingsAction  = deviceMenu->addAction(QIcon(UI_ICON_SCRIPT_SETTINGS), tr("Scripting settings..."), this, SLOT(uiChangeDeviceSettings()));
		ipConfigurationAction = deviceMenu->addAction(QIcon(UI_ICON_EDIT), tr("Set IP Configuration..."), this, SLOT(uiChangeIPConfiguration()));
		
		enterEnvironmentAction = new QAction(QIcon(UI_ICON_ENVIRONMENT_ENTER), tr("Enter environment"), this);
		ui.environmentTree->addAction(enterEnvironmentAction);
		
		enableDeviceAction->setEnabled(device->state == DS_DEACTIVATED);
		disableDeviceAction->setDisabled(device->state == DS_DEACTIVATED);
		ipConfigurationAction->setEnabled(false);
		enterEnvironmentAction->setEnabled(false);
	}
	
	inline void CDeviceOptions::createView() {
		trayIcon = new QSystemTrayIcon(QIcon(iconFile(device)), this);
		
		ui.setupUi(this);
		
		//interfaceDetails = new CInterfaceDetailsModel();
		
//		showTrayCheck = new QCheckBox(tr("Show tray icon for this device"));
		connect(ui.showTrayCheck, SIGNAL(toggled(bool)), trayIcon, SLOT(setVisible(bool)));
		
/*		environmentTree = new QTreeView();
		environmentTree->setContextMenuPolicy(Qt::ActionsContextMenu);
		environmentTree->setAllColumnsShowFocus(true);
		environmentTree->setAlternatingRowColors(true);
		environmentTree->setIconSize(QSize(18, 18));
		environmentTree->setAllColumnsShowFocus(true);*/
		ui.environmentTree->header()->setResizeMode(QHeaderView::ResizeToContents);
		ui.environmentTree->setModel(new CDeviceOptionsModel(device));
		connect(ui.environmentTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(uiSelectionChanged(const QItemSelection &, const QItemSelection &)));
		connect(device, SIGNAL(environmentsUpdated()), ui.environmentTree, SLOT(reset()));
		
		//todo: interfacesänderungen hier
		
/*		statusIcon = new QLabel();
		statusText = new QLabel();
		statusIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);*/
		setHeadInfo();
		
/*		QHBoxLayout * headlayout = new QHBoxLayout();
		headlayout->addWidget(statusIcon);
		headlayout->addWidget(statusText);
		
		QVBo	xLayout * mainlayout = new QVBoxLayout();
		mainlayout->addLayout(headlayout);
		mainlayout->addWidget(environmentTree);
		mainlayout->addWidget(showTrayCheck);
		setLayout(mainlayout);*/
	}
	
	inline void CDeviceOptions::setHeadInfo() {
		ui.iconLabel->setPixmap(QPixmap(iconFile(device)));
		ui.statusLabel->setText(toString(device->state));
	}
	
	void CDeviceOptions::uiHandleTrayActivated(QSystemTrayIcon::ActivationReason reason) {
		if (reason == QSystemTrayIcon::Trigger) {
			showAction->trigger();
		}
	}
	
	void CDeviceOptions::uiShowThisTab() {
		emit showOptions(this);
	}
	
	void CDeviceOptions::uiSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {
		QModelIndexList deselectedIndexes = deselected.indexes();
		QModelIndexList selectedIndexes = selected.indexes();
		
		if (!deselectedIndexes.isEmpty()) {
			QModelIndex targetIndex = deselectedIndexes[0];
			if (!targetIndex.parent().isValid()) {
				CEnvironment * target = (CEnvironment *)(targetIndex.internalPointer());
				disconnect(target, SIGNAL(activeChanged(bool)), enterEnvironmentAction, SLOT(setDisabled(bool)));
				disconnect(enterEnvironmentAction, SIGNAL(triggered()), target, SLOT(enter()));
			}
			
		}
		
		QItemSelectionModel * oldSelectionModel = ui.detailsView->selectionModel();
		QAbstractItemModel * oldItemModel = ui.detailsView->model();
		
		if (!selectedIndexes.isEmpty()) {
			QModelIndex targetIndex = selectedIndexes[0];
			if (!targetIndex.parent().isValid()) {
				CEnvironment * target = (CEnvironment *)(targetIndex.internalPointer());
				connect(target, SIGNAL(activeChanged(bool)), enterEnvironmentAction, SLOT(setDisabled(bool)));
				connect(enterEnvironmentAction, SIGNAL(triggered()), target, SLOT(enter()));
				
				enterEnvironmentAction->setDisabled(target == device->activeEnvironment);
				//workarround für environment details model
				ui.detailsView->setModel(new CInterfaceDetailsModel(NULL));
			}
			else {
				enterEnvironmentAction->setEnabled(false);
				ui.detailsView->setModel(new CInterfaceDetailsModel((CInterface *)(targetIndex.internalPointer())));
			}
		}
		else {
			enterEnvironmentAction->setEnabled(false);
			//workarround für leeres model
			ui.detailsView->setModel(new CInterfaceDetailsModel(NULL));
		}
		
		delete oldSelectionModel;
		delete oldItemModel;
	}
	
	void CDeviceOptions::uiChangeIPConfiguration() {
		CIPConfiguration dialog(this);
		QModelIndex selectedIndex = (ui.environmentTree->selectionModel()->selection().indexes())[0];

		dialog.execute((CInterface *)(selectedIndex.internalPointer()));
	}
	
	void CDeviceOptions::uiChangeDeviceSettings() {
		CScriptSettings dialog(this);
		dialog.execute(this);
	}
	
	void CDeviceOptions::uiHandleStateChange(DeviceState state) {
		setHeadInfo();
		ui.environmentTree->collapseAll();
		if (state == DS_UP)
			ui.environmentTree->expand(ui.environmentTree->model()->index(device->environments.indexOf(device->activeEnvironment), 0));
		
		enableDeviceAction->setEnabled(state == DS_DEACTIVATED);
		disableDeviceAction->setDisabled(state == DS_DEACTIVATED);
		ipConfigurationAction->setEnabled(state == DS_UNCONFIGURED);

		if (!ui.environmentTree->selectionModel()->selectedIndexes().isEmpty()) {
			QModelIndex targetIndex = ui.environmentTree->selectionModel()->selectedIndexes()[0];
			if (!targetIndex.parent().isValid()) {
				CEnvironment * target = (CEnvironment *)(targetIndex.internalPointer());
				enterEnvironmentAction->setDisabled(target == device->activeEnvironment);
			}
			else {
				enterEnvironmentAction->setEnabled(false);
			}
		}
		else {
			enterEnvironmentAction->setEnabled(false);
		}
		if (trayIcon->isVisible()) {
			trayIcon->setToolTip(shortSummary(device));
			trayIcon->setIcon(QIcon(iconFile(device)));
			
			switch (state) {
			case DS_UP:
				emit showMessage(trayIcon, tr("QNUT - %1...").arg(device->name),
					tr("...is now up and running."));
				break;
			case DS_UNCONFIGURED:
				emit showMessage(trayIcon, tr("QNUT - %1...").arg(device->name),
					tr("...got carrier but needs configuration.\n\nKlick here to open the configuration dialog."));
				break;
			case DS_ACTIVATED:
				emit showMessage(trayIcon, tr("QNUT - %1...").arg(device->name),
					tr("...is now activated an waits for carrier."));
				break;
			case DS_DEACTIVATED: 
				emit showMessage(trayIcon, tr("QNUT - %1...").arg(device->name),
					tr("...is now deactivated"));
				break;
			default:
				break;
			}
		}
		else {
			switch (state) {
			case DS_UP:
				emit showMessage(NULL, tr("QNUT"),
					tr("%1 is now up and running.").arg(device->name));
				break;
			case DS_UNCONFIGURED:
				emit showMessage(NULL, tr("QNUT"),
					tr("%1 got carrier but needs configuration.\n\nKlick here to open the configuration dialog.").arg(device->name));
				break;
			case DS_ACTIVATED:
				emit showMessage(NULL, tr("QNUT"),
					tr("%1 is now activated an waits for carrier.").arg(device->name));
				break;
			case DS_DEACTIVATED:
				emit showMessage(NULL, tr("QNUT"),
					tr("%1 is now deactivated").arg(device->name));
				break;
			default:
				break;
			}
		}
		//emit showMessage(tr("QNUT"), tr("%1 changed its state to \"%2\"").arg(device->name, toString(state)), 4000);
		
		if (scriptFlags) {
			QDir workdir(UI_PATH_DEV(device->name));
			bool doExecuteScripts = false;
			QString targetDir;
			switch (state) {
			case DS_UP:
				doExecuteScripts = (scriptFlags & UI_FLAG_SCRIPT_UP);
				targetDir = UI_DIR_SCRIPT_UP;
				break;
			case DS_UNCONFIGURED:
				doExecuteScripts = (scriptFlags & UI_FLAG_SCRIPT_UNCONFIGURED);
				targetDir = UI_DIR_SCRIPT_UNCONFIGURED;
				break;
			case DS_CARRIER:
				doExecuteScripts = (scriptFlags & UI_FLAG_SCRIPT_CARRIER);
				targetDir = UI_DIR_SCRIPT_CARRIER;
				break;
			case DS_ACTIVATED:
				doExecuteScripts = (scriptFlags & UI_FLAG_SCRIPT_ACTIVATED);
				targetDir = UI_DIR_SCRIPT_ACTIVATED;
				break;
			case DS_DEACTIVATED:
				doExecuteScripts = (scriptFlags & UI_FLAG_SCRIPT_DEACTIVATED);
				targetDir = UI_DIR_SCRIPT_DEACTIVATED;
				break;
			default:
				break;
			}
			
			if (doExecuteScripts && workdir.exists(targetDir)) {
				QStringList env;
				QProcess process;
				env << "QNUT_DEV_NAME="  + device->name;
				env << "QNUT_DEV_STATE=" + toString(state);
				//activeEnvironment workarround
				if ((state == DS_UP) && (device->activeEnvironment != NULL)) {
					env << "QNUT_ENV_NAME="  + device->activeEnvironment->name;
					env << "QNUT_IF_COUNT=" + QString::number(device->activeEnvironment->interfaces.count());
					int j = 0;
					foreach (CInterface * i, device->activeEnvironment->interfaces) {
						env << "QNUT_IF_" + QString::number(j) + "=" + i->ip.toString();
						j++;
					}
				}
				process.setEnvironment(env);
				workdir.cd(targetDir);
				foreach(QString i, workdir.entryList()) {
					process.start(workdir.path() + i);
				}
			}
		}
	}
};
