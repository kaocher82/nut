/*
    QNUT - Qt client for the Network UTility server
    Copyright (C) 2010  Oliver Groß <z.o.gross@gmx.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef CABSTRACTWIFINETCONFIGDIALOG_H
#define CABSTRACTWIFINETCONFIGDIALOG_H

#ifndef QNUT_NO_WIRELESS

#include <QDialog>
#include <libnutwireless/cnetworkconfig.h>

class QLineEdit;
class QRegExpValidator;

namespace libnutwireless {
	class CWpaSupplicant;
	class NetconfigStatus;
}

namespace qnut {
	class CAbstractWifiNetConfigDialog : public QDialog {
		Q_OBJECT
	public:
		/**
			* @brief Opens the dialog for adding the given scanned network.
			* @param scanResult scan result with network configuration to use
			*/
		virtual bool execute(libnutwireless::ScanResult scanResult) = 0;
		/**
			* @brief Opens the dialog for configuring the given managed network
			* @param id managed network id
			*/
		virtual bool execute(int id) = 0;
		/// @brief Opens the dialog for adding a new annonymous network
		virtual bool execute() = 0;
		
		/**
		 * @brief Creates the object and initializes some variables.
		 * @param parent parent widget
		 */
		CAbstractWifiNetConfigDialog(libnutwireless::CWpaSupplicant * wpa_supplicant, QWidget * parent = 0);
		/// @brief Destroyes the object.
		~CAbstractWifiNetConfigDialog();
	protected:
		static QRegExpValidator * m_HexValidator;
		static int m_HexValidatorRefs;
		
		libnutwireless::CWpaSupplicant * m_Supplicant;
		
		int m_CurrentID;
		
		static void getConfigErrors(libnutwireless::NetconfigStatus * status, QStringList & errormsg);
		
		static void convertLineEditText(QLineEdit * lineEdit, bool hex);
	protected slots:
		virtual void verifyConfiguration() = 0;
	};
}
#endif // QNUT_NO_WIRELESS

#endif // CABSTRACTWIFINETCONFIGDIALOG_H