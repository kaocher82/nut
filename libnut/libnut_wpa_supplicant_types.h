#ifndef LIBNUT_LIBNUT_WPA_SUPPLICANT_TYPES_H
#define LIBNUT_LIBNUT_WPA_SUPPLICANT_TYPES_H

#include <QString>
#include <QByteArray>
#include <QList>
#include <common/macaddress.h>
#include <QHostAddress>

namespace libnut {

	typedef enum {WNF_NONE=0, WNF_CURRENT=1} wps_network_flags;

	typedef enum {WC_UNDEFINED=0, WC_NONE=1, WC_CCMP=2, WC_TKIP=4, WC_WEP104=8, WC_WEP40=16, WC_WEP=24} wps_ciphers;
	typedef enum {WA_UNDEFINED=0, WA_PLAIN=1,WA_WPA_PSK=2,WA_WPA2_PSK=4, WA_WPA_EAP=8, WA_WPA2_EAP=16, WA_IEEE8021X=32, WA_DEF=62} wps_authentication;
	//WA_WPA_PSK = WAA_OPEN && WKI_WPA && KEYMGMT_WPA_PSK && (GCI_CCMP || GCI_TKIP) && (PCI_CCMP || PCI_TKIP)
	//WA_WPA2_PSK = WAA_OPEN && WKI_RSN && KEYMGMT_WPA_PSK && (GCI_CCMP || GCI_TKIP) && (PCI_CCMP || PCI_TKIP)
	//WA_WPA_EAP = (WAA_OPEN || WAA_LEAP) && KEYMGMT_WPA_EAP && WKI_WPA && (GCI_CCMP || GCI_TKIP) && (PCI_CCMP || PCI_TKIP)
	//WA_WPA2_EAP = (WAA_OPEN || WAA_LEAP) && KEYMGMT_WPA_EAP && WKI_RSN && (GCI_CCMP || GCI_TKIP) && (PCI_CCMP || PCI_TKIP)
	//WA_IEEE8021X = (WAA_OPEN || LEAP) && KEYMGMT_IEEE8021X && (WKI_RSN || WKI_WPA) // This is not very clear yet


	typedef enum {WP_UNDEFINED=0, WP_WPA=1, WP_RSN=2, WP_DEF=3} wps_protocols; //RSN=WPA2
	typedef enum {WGC_UNDEFINED=0, WGC_CCMP=2, WGC_TKIP=4, WGC_WEP104=8, WGC_WEP40=16, WGC_DEF=30} wps_group_ciphers;
	typedef enum {WPC_UNDEFINED=0, WPC_NONE=1, WPC_CCMP=2, WPC_TKIP=4, WPC_DEF=6} wps_pairwise_ciphers;
	typedef enum {WKM_UNDEFINED=0, WKM_NONE=1, WKM_WPA_PSK=2, WKM_WPA_EAP=4, WKM_IEEE8021X=8, WKM_DEF=6} wps_key_management;
	typedef enum {WAA_UNDEFINED=0, WAA_OPEN=1, WAA_SHARED=2, WAA_LEAP=4} wps_auth_algs; //Default: automatic selection

	typedef enum {WI_MSG, WI_REQ,WI_EVENT} wps_interact_type;
	typedef enum {WR_FAIL, WR_PASSWORD, WR_IDENTITY, WR_NEW_PASSWORD, WR_PIN, WR_OTP, WR_PASSPHRASE} wps_req_type;
	typedef enum {WE_OTHER, WE_DISCONNECTED, WE_CONNECTED, WE_TERMINATING, WE_PASSWORD_CHANGED, WE_EAP_NOTIFICATION, WE_EAP_STARTED, WE_EAP_METHOD, WE_EAP_SUCCESS, WE_EAP_FAILURE } wps_event_type;

	typedef enum {EAPF_UNDEFINED=-1, EAPF_WIRED=0,EAPF_DYN_UNICAST_WEP=1, EAPF_BCAST_WEP=2,EAPF_DEFAULT=3} wps_eapol_flags;

	//Default: all build in
	typedef enum {EAPM_UNDEFINED=0, EAPM_MD5=1,EAPM_MSCHAPV2=2,EAPM_OTP=4,EAPM_GTC=8,EAPM_TLS=16,EAPM_PEAP=32,EAPM_TTLS=64,EAPM_ALL=127} wps_eap_method;

	//0001 = 1
	//0010 = 2
	//0100 = 4
	//0100 = 8

	typedef enum {
	WCF_NONE=0, WCF_SSID=1,WCF_BSSID=2,WCF_DISABLED=4,WCF_ID_STR=8,
	WCF_SCAN_SSID=16, WCF_PRIORITY=32, WCF_MODE=64, WCF_FREQ=128,
	WCF_PROTO=256, WCF_KEYMGMT=512, WCF_AUTH_ALG=1024, WCF_PAIRWISE=2048,
	WCF_GROUP=0x0000001000, WCF_PSK=0x0000002000, WCF_EAPOL_FLAGS=0x0000004000, WCF_MIXED_CELL=0x0000008000,
	WCF_PROA_KEY_CACHING=0x00000010000, WCF_WEP_KEY0=0x0000020000, WCF_WEP_KEY1=0x0000040000, WCF_WEP_KEY2=0x0000080000,
	WCF_WEP_KEY3=0x0000100000, WCF_WEP_KEY_IDX=0x0000200000, WCF_PEERKEY=0x0000400000, WCF_ALL=0x00007FFFFF
	}wps_netconfig_failures;

	typedef enum {
	WECF_NONE=0, WECF_EAP=1,WECF_IDENTITY=2,WECF_ANON_IDENTITY=4,WECF_PASSWD=8,
	WECF_CA_CERT=16, WECF_CA_PATH=32, WECF_CLIENT_CERT=64, WECF_PRIVATE_KEY=128,
	WECF_PRIVATE_KEY_PASSWD=256, WECF_DH_FILE=512, WECF_SUBJECT_MATCH=1024, WECF_ALTSUBJECT_MATCH=2048,
	WECF_PHASE1=0x0000001000, WECF_PHASE2=0x0000002000, WECF_CA_CERT2=0x0000004000, WECF_CA_PATH2=0x0000008000,
	WECF_CLIENT_CERT2=0x00000010000, WECF_PRIVATE_KEY2=0x0000020000,
	WECF_PRIVATE_KEY2_PASSWD=0x0000040000, WECF_DH_FILE2=0x0000080000, WECF_SUBJECT_MATCH2=0x0000100000,
	WECF_ALTSUBJECT_MATCH2=0x0000200000, WECF_FRAGMENT_SIZE=0x0000400000, WECF_EAPPSK=0x0000800000,
	WECF_NAI=0x00001000000, WECF_PAC_FILE=0x00002000000, WECF_ALL=0x00003FFFFFF
	} wps_eap_netconfig_failures;

	struct wps_netconfig_status {
		wps_netconfig_failures failures;
		wps_eap_netconfig_failures eap_failures;
		int id;
	};

	struct wps_scan {
		nut::MacAddress bssid;
		QString ssid;
		int freq;
		int level;
		wps_ciphers ciphers;
		wps_key_management keyManagement;
		wps_protocols protocols;
	};
	struct wps_variable;
	typedef QList<wps_variable> wps_MIB;
	//enums are NOT complete, but maybe we schould change this to QString
	struct wps_status {
// 		typedef enum {COMPLETED} WPA_STATE;
// 		typedef enum {AUTHENTICATED} PAE_STATE;
// 		typedef enum {AUTHORIZED} PORT_STATUS;
// 		typedef enum {AUTO} PORT_CONTROL;
// 		typedef enum {IDLE} BACKEND_STATE;
// 		typedef enum {SUCCESS} EAP_STATE;
// 		typedef enum {NOSTATE} METHOD_STATE;
// 		typedef enum {COND_SUCC} DECISION;
		//These typedefs may change in the future to the ones above (more complete)
		typedef QString WPA_STATE;
		typedef QString PAE_STATE;
		typedef QString PORT_STATUS;
		typedef QString PORT_CONTROL;
		typedef QString BACKEND_STATE;
		typedef QString EAP_STATE;
		typedef QString METHOD_STATE;
		typedef QString DECISION;
		nut::MacAddress bssid;
		QString ssid;
		int id;
		wps_pairwise_ciphers pairwise_cipher;
		wps_group_ciphers group_cipher;
		wps_key_management key_mgmt;
		WPA_STATE wpa_state;
		QHostAddress ip_address;
		PAE_STATE pae_state;
		PORT_STATUS PortStatus;
		int heldPeriod;
		int authPeriod;
		int startPeriod;
		int maxStart;
		PORT_CONTROL portControl;
		BACKEND_STATE backend_state;
		EAP_STATE eap_state;
		int reqMethod;
		METHOD_STATE methodState;
		DECISION decision;
		int ClientTimeout;
	};

	struct wps_variable {
		typedef enum {PLAIN=1,STRING=2,NUMBER=4,LOGIC=8} wps_variable_type;
		wps_variable_type type;
		QString name;
		union {
			qint32 * num;
			QString * str;
			bool * logic;
		} value;
	};
	struct wps_net_var {
		typedef enum {PLAIN=1,STRING=2,NUMBER=4,LOGIC=8} Type;
		QString name;
		union {
			int * num;
			QString * str;
			bool * logic;
		} value;
	};
	struct wps_req {
		wps_req_type type;
		int id;
	};


	//Conversion functions
	wps_group_ciphers toGroupCiphers(wps_ciphers cip);
	wps_pairwise_ciphers toPairwiseCiphers(wps_ciphers cip);
	wps_key_management toKeyManagment(wps_authentication auth);
	wps_auth_algs toAuthAlgs(wps_authentication auth);
	wps_protocols toProtocols(wps_authentication auth);


	QString toString(wps_group_ciphers cip);
	QString toString(wps_pairwise_ciphers cip);
	QString toString(wps_key_management keym);
	QString toString(wps_auth_algs algs);
	QString toString(wps_protocols proto);


	QString toString(wps_ciphers cip);
	QString toString(wps_authentication auth);
	QString toString(wps_req_type reqt);
	
	QString toString(wps_eapol_flags flags);
	QString toString(wps_eap_method method);
	

	
	struct wps_network {
		int id;
		QString ssid;
		nut::MacAddress bssid;
		wps_network_flags flags;
	};

	class wps_eap_network_config {
		public:
			wps_eap_network_config();
			~wps_eap_network_config();
			//Following fields are only used with internal EAP implementation.
			wps_eap_method eap; //space-separated list of accepted EAP methods TODO: implement
			QString identity; //Identity string for EAP
			QString anonymous_identity; //Anonymous identity string for EAP;
			QString password; //Password string for EAP.
			QString ca_cert; //File path to CA certificate file (PEM/DER).
			QString ca_path; //Directory path for CA certificate files (PEM).
			QString client_cert; //File path to client certificate file (PEM/DER)
			QString private_key; //File path to client private key file (PEM/DER/PFX)
			QString private_key_passwd; //Password for private key file
			QString dh_file; //File path to DH/DSA parameters file (in PEM format)
			QString subject_match; //Substring to be matched against the subject of the authentication server certificate.
			QString altsubject_match; //Semicolon separated string of entries to be matched against the alternative subject name of the authentication server certificate.
			QString phase1; //Phase1 (outer authentication, i.e., TLS tunnel) parameters (string with field-value pairs, e.g., "peapver=0" or	"peapver=1 peaplabel=1")
			QString phase2; //Phase2 (inner authentication with TLS tunnel) parameters
			QString ca_cert2; //File path to CA certificate file.
			QString ca_path2; //Directory path for CA certificate files (PEM)
			QString client_cert2; //File path to client certificate file
			QString private_key2; //File path to client private key file
			QString private_key2_passwd; //Password for private key file
			QString dh_file2; //File path to DH/DSA parameters file (in PEM format)
			QString subject_match2; //Substring to be matched against the subject of the authentication server certificate.
			QString altsubject_match2; //Substring to be matched against the alternative subject name of the authentication server certificate.
			int fragment_size; //Maximum EAP fragment size in bytes (default 1398);
			QString eappsk; //16-byte (128-bit, 32 hex digits) pre-shared key in hex format
			QString nai; //user NAI
			QString pac_file; //File path for the PAC entries.
	};

	class wps_network_config { //All without linebreak
		public:
			wps_network_config();
			~wps_network_config();
			QString ssid;
			nut::MacAddress bssid;
			bool disabled;
			QString id_str; // Network identifier string for external scripts
			bool scan_ssid; // (do not) scan with SSID-specific Probe Request frames
			int priority;
			bool mode; //0 = infrastructure (Managed) mode, i.e., associate with an AP (default) 1 = IBSS (ad-hoc, peer-to-peer)
			int frequency;
			wps_protocols protocols; //list of accepted protocols TODO: implement
			wps_key_management keyManagement; // list of accepted authenticated key management protocols
			wps_auth_algs auth_alg; //list of allowed IEEE 802.11 authentication algorithms TODO:implement
			wps_pairwise_ciphers pairwise; //list of accepted pairwise (unicast) ciphers for WPA (CCMP,TKIP,NONE)
			wps_group_ciphers group; //list of accepted group (broadcast/multicast) ciphers for WPA (CCMP;TKIP;WEP104/40)
			QString psk; //WPA preshared key; 256-bit pre-shared key
			wps_eapol_flags eapol_flags; // IEEE 802.1X/EAPOL options (bit field) TODO:implement
			bool mixed_cell; //This option can be used to configure whether so called mixed
			bool proactive_key_caching; //Enable/disable opportunistic PMKSA caching for WPA2.
			QString wep_key0; //Static WEP key (ASCII in double quotation, hex without)
			QString wep_key1;
			QString wep_key2;
			QString wep_key3;
			char wep_tx_keyidx; //Default WEP key index (TX) (0..3) TODO: implement
			bool peerkey; //Whether PeerKey negotiation for direct links (IEEE 802.11e DLS) is allowed.
			wps_eap_network_config eap_config;
	};

}
#endif
