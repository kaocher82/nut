//
// C++ Implementation: environmenttreemodel
//
// Description: 
//
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QIcon>
#include "environmenttreemodel.h"
#include "constants.h"

#define DEVOP_MOD_ITEM    0
#define DEVOP_MOD_STATUS  1
#define DEVOP_MOD_IP      2
#define DEVOP_MOD_NETMASK 3
#define DEVOP_MOD_GATEWAY 4
#define DEVOP_MOD_CONFIG  5

namespace qnut {
	using namespace nut;
	CEnvironmentTreeModel::CEnvironmentTreeModel(CDevice * data, QObject * parent) : QAbstractItemModel(parent) {
		device = data;
		if (data) {
			//connect(device, SIGNAL(environmentsUpdated()), this, SLOT(layoutChanged()));
			foreach(CEnvironment * environment, device->environments) {
				//connect(environment, SIGNAL(activeChanged(bool)), this, SIGNAL(layoutChanged()));
				foreach(CInterface * interface, environment->interfaces) {
					connect(interface, SIGNAL(stateChanged(InterfaceState)), this, SIGNAL(layoutChanged()));
				}
			}
		}
	}
	
	CEnvironmentTreeModel::~CEnvironmentTreeModel() {
		device = NULL;
	}
	
	int CEnvironmentTreeModel::columnCount(const QModelIndex &) const {
		if (device == NULL)
			return 0;
		else
			return 3;
	}
	
	int CEnvironmentTreeModel::rowCount(const QModelIndex & parent) const {
		if (device == NULL)
			return 0;
		
		if (!parent.isValid())
			return device->environments.count();
		else {
			QObject * parentData = (QObject *)(parent.internalPointer());
			if (parentData->parent() == device)
				return ((CEnvironment *)parentData)->interfaces.count();
			else
				return 0;
		}
	}
	
	QVariant CEnvironmentTreeModel::data(const QModelIndex & index, int role) const {
		if (device == NULL)
			return QVariant();
		
		if (!index.isValid())
			return QVariant();
		
		QObject * data = (QObject *)(index.internalPointer());
		switch (index.column()) {
			case DEVOP_MOD_ITEM:
				if (data->parent() == device) {
					switch (role) {
					case Qt::DisplayRole:
						return ((CEnvironment *)data)->name;
					case Qt::DecorationRole:
						return QIcon(UI_ICON_ENVIRONMENT);
					default:
						break;
					}
				}
				
				if (data->parent()->parent() == device) {
					CInterface * interface = (CInterface *)data;
					CEnvironment * environment = (CEnvironment *)(((QObject *)interface)->parent());
					switch (role) {
					case Qt::DisplayRole:
						return '#' + QString::number(environment->interfaces.indexOf(interface));// +
							//' ' + (interface->isStatic ? tr("static") : tr("dynamic"));
					case Qt::DecorationRole:
						return QIcon(UI_ICON_INTERFACE);
					default:
						break;
					}
				}
				break;
			case DEVOP_MOD_STATUS:
				if (role == Qt::DisplayRole) {
					if (data->parent() == device) {
						return (static_cast<CEnvironment *>(data) == device->activeEnvironment) ? tr("active") : QString('-');
					}
					
					if (data->parent()->parent() == device) {
						switch (static_cast<CInterface *>(data)->state) {
						case IFS_OFF:
							return tr("off");
						case IFS_STATIC:
							return tr("static");
						case IFS_DHCP:
							return tr("dynamic");
						case IFS_ZEROCONF:
							return tr("zeroconf");
						default:
							break;
						}
					}
				}
				break;
			case DEVOP_MOD_IP:
				if (role == Qt::DisplayRole) {
					if (data->parent() == device) {
						return QString('-');
					}
					
					if (data->parent()->parent() == device) {
						if (static_cast<CInterface *>(data)->state == IFS_OFF) {
							if (static_cast<CInterface *>(data)->getConfig().getFlags() & IPv4Config::DO_DHCP) {
								return tr("none");
							}
							else if (static_cast<CInterface *>(data)->getConfig().getFlags() & IPv4Config::DO_STATIC) {
								return static_cast<CInterface *>(data)->getConfig().getStaticIP().toString();
							}
							else
								return tr("unknown");
						}
						else
							return static_cast<CInterface *>(data)->ip.toString();
					}
				}
				break;
			default:
				break;
		}
		
		return QVariant();
	}
	
	Qt::ItemFlags CEnvironmentTreeModel::flags(const QModelIndex & index) const {
		if (device == NULL)
			return 0;
		
		if (!index.isValid())
			return 0;
		
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	
	QVariant CEnvironmentTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
		if (device == NULL)
			return QVariant();
		
		if (role != Qt::DisplayRole)
			return QVariant();
		
		if (orientation == Qt::Horizontal) {
			switch (section) {
			case DEVOP_MOD_ITEM:
				return tr("Item");
			case DEVOP_MOD_STATUS:
				return tr("Status");
			case DEVOP_MOD_CONFIG:
				return tr("Config");
			case DEVOP_MOD_IP:
				return tr("IP-Address");
			case DEVOP_MOD_NETMASK:
				return tr("Netmask");
			case DEVOP_MOD_GATEWAY:
				return tr("Gateway");
			default:
				break;
			}
		}
		return QVariant();
	}
	
	QModelIndex CEnvironmentTreeModel::index(int row, int column, const QModelIndex & parent) const {
		if (device == NULL)
			return QModelIndex();
		
		if (!hasIndex(row, column, parent))
			return QModelIndex();
		
		
		if (!parent.isValid()) {
			if (row < device->environments.count())
				return createIndex(row, column, device->environments[row]);
		}
		else {
			QObject * parentData = (QObject *)(parent.internalPointer());
			if ((parentData->parent() == device) && (row < ((CEnvironment *)(parentData))->interfaces.count()))
				return createIndex(row, column, ((CEnvironment *)(parent.internalPointer()))->interfaces[row]);
		}
		
		return QModelIndex();
	}
	
	QModelIndex CEnvironmentTreeModel::parent(const QModelIndex & index) const {
		if (device == NULL)
			return QModelIndex();
		
		if (!index.isValid())
			return QModelIndex();
		
		if (index.internalPointer() == NULL)
			return QModelIndex();
		
		QObject * parentData = ((QObject *)(index.internalPointer()))->parent();
		
		if (parentData->parent() == device)
			return createIndex(device->environments.indexOf((CEnvironment *)(parentData)), 0, (void *)(parentData));
		else
			return QModelIndex();
		
	}
};