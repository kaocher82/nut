#ifndef LIBNUTWIRELESS_TYPES_H
#define LIBNUTWIRELESS_TYPES_H

#pragma once

#include <QString>

namespace libnutwireless {
	/** Enum of possible protocols */
	enum Protocols {
		PROTO_UNDEFINED = 0,
		PROTO_WPA = 1, // "Microsoft" vendor extension
		PROTO_RSN = 2, // RSN=WPA2
		PROTO_DEFAULT = 3,
	};

	/** Enum of possible group ciphers. */
	enum GroupCiphers {
		GCI_UNDEFINED = 0,
		GCI_NONE = 2,
		GCI_WEP40 = 4,
		GCI_WEP104 = 8,
		GCI_TKIP = 16,
		GCI_CCMP = 32,
		GCI_WRAP = 64,
		GCI_DEF = 60,
	};

	/** Enum of possible pairwise ciphers */
	enum PairwiseCiphers {
		PCI_UNDEFINED = 0,
		PCI_NONE = 1,
		PCI_TKIP = 2,
		PCI_CCMP = 4,
		PCI_DEF = 6,
	};

	/** Enum of possible key management */
	enum KeyManagement {
		KM_UNDEFINED = 0,
		KM_NONE = 4,
		KM_WPA_NONE = 8,
		KM_WPA_PSK = 16,
		KM_WPA_EAP = 32,
		KM_IEEE8021X = 64,
		KM_DEF = 48,
	}; //TODO:change parsers due to KM_OFF

	/** Enum of possible authentication algorithms */
	enum AuthenticationAlgs {
		AUTHALG_UNDEFINED = 0,
		AUTHALG_OPEN = 1,
		AUTHALG_SHARED = 2,
		AUTHALG_LEAP = 4,
	}; //Default: automatic selection

	/** Possible events from wpa_supplicant, not complete */
	enum EventType {
		EVENT_OTHER,
		EVENT_DISCONNECTED,
		EVENT_CONNECTED,
		EVENT_TERMINATING,
		EVENT_PASSWORD_CHANGED,
		EVENT_EAP_NOTIFICATION,
		EVENT_EAP_STARTED,
		EVENT_EAP_METHOD,
		EVENT_EAP_SUCCESS,
		EVENT_EAP_FAILURE,
	};

	/** Eapol flags */
	enum EapolFlags {
		EAPF_UNDEFINED = -1,
		EAPF_WIRED = 0,
		EAPF_DYN_UNICAST_WEP = 1,
		EAPF_BCAST_WEP = 2,
		EAPF_DEFAULT = 3,
	};

	/** List of Eap methods for wpa_supplicant */
	enum EapMethod {
		EAPM_UNDEFINED = 0,
		EAPM_MD5 = 1,
		EAPM_MSCHAPV2 = 2,
		EAPM_OTP = 4,
		EAPM_GTC = 8,
		EAPM_TLS = 16,
		EAPM_PEAP = 32,
		EAPM_TTLS = 64,
		EAPM_ALL = 127,
		EAPM_AKA = 128,
		EAPM_FAST = 256,
		EAPM_LEAP = 512,
		EAPM_PSK = 1024,
		EAPM_PAX = 2048,
		EAPM_SAKE = 4096,
		EAPM_GPSK = 8192,
	};

	/** QOOL is a tri-state. Its name is derived from Bool and qubit. */
	enum QOOL {
		QOOL_UNDEFINED = -1,
		QOOL_FALSE = 0,
		QOOL_TRUE = 1,
	}; // Like a qubit :)

	/** Enum of operation modes of wireless networks (see WirelessExtension) */
	enum OPMODE {
		OPM_AUTO = 0,
		OPM_ADHOC = 1,
		OPM_MANAGED = 2,
		OPM_MASTER = 3,
		OPM_REPEATER = 4,
		OPM_SECONDARY = 5,
		OPM_MONITOR = 6,
		OPM_UNKNOWN_BUG = 7,
	};

	//Conversion functions
	Protocols toProtocols(QString const& str);
	KeyManagement toKeyMgmt(QString const& str);
	AuthenticationAlgs toAuthAlg(QString const& str);
	PairwiseCiphers toPairwiseCiphers(QString const& str);
	GroupCiphers toGroupCiphers(QString const& str);
	EapolFlags toEapolFlags(QString const& str);
	EapMethod toEapMethod(QString const& str);

	///Convert GroupCiphers to QString
	QString toString(GroupCiphers cip);
	///Convert PairwiseCiphers to QString
	QString toString(PairwiseCiphers cip);
	///Convert KeyManagement to QString
	QString toString(KeyManagement keym);
	///Convert AuthenticationAlgs to QString
	QString toString(AuthenticationAlgs algs);
	///Convert Protocols to QString
	QString toString(Protocols proto);

	///Convert EapolFlags to QString
	QString toString(EapolFlags flags);
	///Convert EapMethod to QString
	QString toString(EapMethod method);

	///Convert QOOL to QString (QOOL_UNDEFINED="-1", QOOL_FALSE="0", QOOL_TRUE="1")
	QString toNumberString(QOOL b);
	///Convert QOOL to QString (QOOL_UNDEFINED="undefined", QOOL_FALSE="false", QOOL_TRUE="true")
	QString toString(QOOL b);
	///Convert QOOL to bool (QOOL_UNDEFINED=false, QOOL_FALSE=false, QOOL_TRUE=true)
	bool toBool(QOOL b);
	///Convert bool to QOOL (false=QOOL_FALSE, true=QOOL_TRUE)
	QOOL toQOOL(bool b);
	///Convert int to QOOL
	QOOL toQOOL(int i);

	/// Convert bool to number (false=0; true=1)
	inline int toNumber(bool b) {
		return (b ? 1 : 0);
	}

	/// Convert QOOL to number (QOOL_UNDEFINED=-1, QOOL_FALSE=0, QOOL_TRUE=1)
	inline int toNumber(QOOL b) {
		return static_cast<int>(b);
	}

	/// Convert QString to bool ("1" = true; otherwise false)
	bool toBool(QString const& str);

	/// Convert QString to QOOL ("-1"=QOOL_UNDEFINED, "0"=QOOL_FALSE, "1"=QOOL_TRUE)
	QOOL toQOOL(QString const& str);
}

#endif /* LIBNUTWIRELESS_TYPES_H */
