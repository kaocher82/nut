//
// C++ Interface: availableapmodel
//
// Description: 
//
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QNUT_AVAILABLEAPMODEL_H
#define QNUT_AVAILABLEAPMODEL_H

#include <QAbstractItemModel>
#include <libnut/libnut_cli.h>

namespace qnut {
	using namespace libnut;

	class CAvailableAPModel : public QAbstractItemModel {
		Q_OBJECT
	public:
		CAvailableAPModel(CWpa_Supplicant * data = NULL, QObject * parent = 0);
		~CAvailableAPModel();
		
		QVariant data(const QModelIndex & index, int role) const;
		Qt::ItemFlags flags(const QModelIndex & index) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex & index) const;
		int rowCount(const QModelIndex & parent = QModelIndex()) const;
		int columnCount(const QModelIndex & parent = QModelIndex()) const;
	public slots:
		void reloadScans();
	private:
		CWpa_Supplicant * supplicant;
		QList<wps_scan> scans;
	};
}

#endif