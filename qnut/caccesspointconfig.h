//
// C++ Interface: CAccessPointConfig
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef QNUT_ACCESSPOINTCONFIG_H
#define QNUT_ACCESSPOINTCONFIG_H

#ifndef NUT_NO_WIRELESS
#include "cabstractwifinetconfigdialog.h"

#include <libnutwireless/cnetworkconfig.h>
#include <QSignalMapper>

#include "ui_accesspointconfig.h"

namespace qnut {
	/**
	 * @brief CAccessPointConfig provides a dialog to configure a managed nework or add a new network for the given wpa_supplicant.
	 * @author Oliver Groß <z.o.gross@gmx.de>
	 *
	 * On creation, the CAccessPointConfig sets up the user interface according to the given instance of a wpa_supplicant.
	 * The class provides function to execute the dialog for adding (w/o scan result) and configuring a network (by network id).
	 *
	 * By accepting the settings made in the UI they are verified (on error the dialog stays open).
	 */
	class CAccessPointConfig final : public CAbstractWifiNetConfigDialog {
		Q_OBJECT
	public:
		static QString lastFileOpenDir() { return m_LastFileOpenDir; }
		static void setLastFileOpenDir(QString value) { m_LastFileOpenDir = value; }

		/**
		 * @brief Creates the object and initializes the basic user interface.
		 * @param parent parent widget
		 */
		explicit CAccessPointConfig(libnutwireless::CWireless* interface, QWidget* parent = nullptr);
		virtual ~CAccessPointConfig();
	protected:
		Ui::apconf ui;

		struct FileEditStrings {
			QString title;
			QString filter;
		};

		static QString m_LastFileOpenDir;

		QButtonGroup * m_EAPPhaseButtons;
		QSignalMapper * m_FileEditMapper;
		QMap<QWidget *, FileEditStrings> m_FileSelectStringMap;

		inline void writeEAPPhaseConfig(libnutwireless::CNetworkConfig & eap_config, int phase);
		inline void writeEAPConfig(libnutwireless::CNetworkConfig & eap_config);
		inline void readEAPPhaseConfig(libnutwireless::CNetworkConfig & eap_config, int phase);
		inline void readEAPConfig(libnutwireless::CNetworkConfig & eap_config);
		inline void updateWEPState(int keyMode, int rsnMode);
	protected slots:
		void setAuthConfig(int type);
		void handleRSNModeChanged(int value);

		void toggleSSIDHex(bool checked);

		bool applyConfiguration() override;

		void countPskChars(QString psk);
		void togglePlainPSK(bool show);

		void selectFile(QWidget * reciever);

		void setUiEAPPhase(int phase);

		void populateUi() override;
	private:
		CErrorCodeEvaluator * m_EapErrorCodeEvaluator;
		inline void populateErrorCodeEvaluator();
	};
}
#endif

#endif
