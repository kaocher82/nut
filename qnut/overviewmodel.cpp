//
// C++ Implementation: overviewlistmodel
//
// Description: 
// Dies stellt die Implementierung für das Model des ListView Steuerelemnts dar.
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QIcon>
#include "overviewmodel.h"
#include "common.h"

#define OV_MOD_NAME    0
#define OV_MOD_STATUS  1
#define OV_MOD_TYPE    2
#define OV_MOD_ENV     3
#define OV_MOD_IP      4
#define OV_MOD_SIG     5

namespace qnut {
	COverViewModel::COverViewModel(CDeviceManager * deviceManager, QObject * parent) : QAbstractItemModel(parent) {
		if (deviceManager) {
			devices = &(deviceManager->devices);
			
			connect(deviceManager, SIGNAL(deviceAdded(CDevice *)), this, SLOT(deviceAdded(CDevice *)));
			connect(deviceManager, SIGNAL(deviceRemoved(CDevice *)), this, SLOT(deviceRemoved(CDevice *)));
		}
		else
			devices = NULL;
	}
	
	COverViewModel::~COverViewModel() {
		devices = NULL;
	}
	
	void COverViewModel::deviceAdded(CDevice * device) {
		connect(device, SIGNAL(stateChanged(DeviceState)), this, SIGNAL(layoutChanged()));
		if (device->type == DT_AIR)
			connect(device->wpa_supplicant, SIGNAL(signalQualityUpdated()), this, SIGNAL(layoutChanged()));
		emit layoutChanged();
	}
	
	void COverViewModel::deviceRemoved(CDevice * device) {
		disconnect(device, SIGNAL(stateChanged(DeviceState)), this, SIGNAL(layoutChanged()));
		if (device->type == DT_AIR)
			disconnect(device->wpa_supplicant, SIGNAL(signalQualityUpdated()), this, SIGNAL(layoutChanged()));
		emit layoutChanged();
	}
	
	Qt::ItemFlags COverViewModel::flags(const QModelIndex & index) const {
		if (devices == NULL)
			return 0;
		
		if (!index.isValid())
			return 0;
		
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	
	QModelIndex COverViewModel::parent(const QModelIndex &) const {
		return QModelIndex();
	}
	
	int COverViewModel::columnCount(const QModelIndex & parent) const {
		if (devices == NULL)
			return 0;
			
		if (parent.isValid())
			return 0;
		
		return 6;
	}
	
	QModelIndex COverViewModel::index(int row, int column, const QModelIndex & parent) const {
		if ((devices == NULL) || (parent.isValid()) || (row >= devices->size()))
			return QModelIndex();
		else
			return createIndex(row, column, (void *)(devices->at(row)));
	}
	
	int COverViewModel::rowCount(const QModelIndex & parent) const {
		if (devices == NULL)
			return 0;
		
		if (!parent.isValid())
			return devices->count();
		
		return 0;
	}
	
	QVariant COverViewModel::data(const QModelIndex & index, int role) const {
		if (devices == NULL)
			return QVariant();
		
		if (!index.isValid())
			return QVariant();
		
/*		if (index.row() >= devices->size())
			return QVariant();*/
		
		CDevice * data = (CDevice *)(index.internalPointer());
		
		if (role == Qt::DisplayRole) {
			switch (index.column()) {
			case OV_MOD_NAME:
				return data->name;
			case OV_MOD_STATUS:
				return toString(data->state);
			case OV_MOD_TYPE:
				return toString(data->type);
			case OV_MOD_IP: {
					if (data->state != DS_UP)
						return QString('-');
					else
						return activeIP(data);
				}
			case OV_MOD_ENV:
				if (data->state >= DS_UNCONFIGURED)
					return data->activeEnvironment->name;
				else
					return tr("none");
			case OV_MOD_SIG:
				if (data->type == DT_AIR) {
					return signalSummary(data->wpa_supplicant->getSignalQuality());
				}
				else
					return QString('-');
			default:
				break;
			}
		}
		else if (role == Qt::DecorationRole) {
			if (index.column() == 0) {
				return QIcon(iconFile(data));
			}
		}
		return QVariant();
	}
	
	QVariant COverViewModel::headerData(int section, Qt::Orientation orientation, int role) const {
		if (devices == NULL)
			return QVariant();
		
		if (role != Qt::DisplayRole)
			return QVariant();
		
		if (orientation == Qt::Horizontal) {
			switch (section) {
			case OV_MOD_NAME:
				return tr("Device");
			case OV_MOD_STATUS:
				return tr("Status");
			case OV_MOD_TYPE:
				return tr("Type");
			case OV_MOD_IP:
				return tr("assigned IP-Address");
			case OV_MOD_ENV:
				return tr("Environment");
			case OV_MOD_SIG:
				return tr("Signal (Quality, Level, Noise)");
			default:
				break;
			}
		}
		return QVariant();
	}
};
