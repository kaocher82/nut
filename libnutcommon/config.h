#ifndef NUT_COMMON_CONFIG_H
#define NUT_COMMON_CONFIG_H

#include <memory>
#include <map>

#include <QObject>
#include <QString>
#include <QList>
#include <QVector>
#include <QHostAddress>
#include <QDBusArgument>

#include "macaddress.h"
#include "flags.h"

namespace libnutcommon {
	class Config;
	class DeviceConfig;
	class SelectRule;
	class SelectConfig;
	class EnvironmentConfig;
	class IPv4Config;
}

namespace libnutcommon {
	/** @brief Name, Wildcard or RegExp pattern for a device name
	 * sorts plain value before regexp before wildcard, for use
	 * as a key in std::map; first matching entry should "win".
	 */
	class DeviceNamePattern {
	public:
		enum PatternType {
			Plain = 0,
			RegExp = 1,
			Wildcard = 2,
		};

		QString pattern;
		PatternType type;

		bool operator<(const DeviceNamePattern &other) const;
		bool operator>(const DeviceNamePattern &other) const { return other < *this; }
		bool operator<=(const DeviceNamePattern &other) const { return !(other < *this); }
		bool operator>=(const DeviceNamePattern &other) const { return !(*this < other); }

		/* whether name matches pattern */
		bool match(QString name) const;

		/* human readable "type" */
		QString typeString() const;
	};

	/** @brief Container for all \link DeviceConfig device configs\endlink.
	 */
	class Config {
	public:
		std::map< DeviceNamePattern, std::shared_ptr<DeviceConfig> > namedDeviceConfigs;

		/* create (overwrite) mapping for pattern with a new empty config; return config reference */
		std::shared_ptr<DeviceConfig> create(const DeviceNamePattern& pattern);

		/* lookup first matching pattern and return associated config reference */
		std::shared_ptr<DeviceConfig> lookup(QString deviceName);
	};

	/** @brief Each device has a list of \link EnvironmentConfig Environments\endlink and some additional config values.
	 *
	 * Each device can have many Environments, but it can only be in one; each
	 * Environment consists of zero, one or more Interfaces.
	 *
	 * Which Environment is taken is controlled by the SelectRules of an Environment.
	 *
	 * For WLAN devices supported by wpa_supplicant you can specify a configfile and a driver (wext by default).
	 */
	class DeviceConfig {
	public:
		std::vector< std::shared_ptr<EnvironmentConfig> > environments;
		bool noAutoStart = false;
		QString wpaConfigFile;
		QString wpaDriver;
		int gateway_metric = -1;
	};
	QDBusArgument &operator<< (QDBusArgument &argument, const DeviceConfig &data);
	const QDBusArgument &operator>> (const QDBusArgument &argument, DeviceConfig &data);

	/** @brief Result type of a select test.
	 * (formally a boolean expression with one variable "User")
	 */
	enum class SelectResult : quint8 {
		False = 0,    //!< Cannot select
		User = 1,     //!< Do select only if user does want it
		NotUser = 2,  //!< Should not happen; "only select if user does not want it..." is not good
		True = 3,     //!< Select
	};
	bool evaluate(SelectResult a, bool user);
	SelectResult operator||(SelectResult a, SelectResult b);
	SelectResult operator&&(SelectResult a, SelectResult b);
	SelectResult operator^(SelectResult a, SelectResult b);
	SelectResult operator!(SelectResult a);
	QDBusArgument &operator<< (QDBusArgument &argument, SelectResult selectResult);
	const QDBusArgument &operator>> (const QDBusArgument &argument, SelectResult &selectResult);

	enum class SelectType : quint8 {
		USER = 0,   //!< Select if user does want it (=> return SelectResult::User)
		ARP,        //!< Select if ipAddr is found on the network from the device; if macAddr != 0 it is matched too.
		ESSID,      //!< Select if in wlan essid;
		AND_BLOCK,  //!< Select a list of \link SelectRule SelectRules\endlink, results combined with AND
		OR_BLOCK,   //!< Select a list of \link SelectRule SelectRules\endlink, results combined with OR
	};
	QDBusArgument &operator<< (QDBusArgument &argument, SelectType selectType);
	const QDBusArgument &operator>> (const QDBusArgument &argument, SelectType &selectType);

	/** @brief A select operation.
	 */
	class SelectRule {
	public:
		SelectRule() : invert(false), selType(SelectType::USER) { }
		SelectRule(const QHostAddress &ipAddr, bool invert = false) : invert(invert), selType(SelectType::ARP), ipAddr(ipAddr) { }
		SelectRule(const QHostAddress &ipAddr, const libnutcommon::MacAddress &macAddr, bool invert = false) : invert(invert), selType(SelectType::ARP), ipAddr(ipAddr), macAddr(macAddr) { }
		SelectRule(const QString &essid, bool invert = false) : invert(invert), selType(SelectType::ESSID), essid(essid) { }
		SelectRule(quint32 block, SelectType blockType, bool invert = false) : invert(invert), selType(blockType), block(block) { }

		bool invert = false;        //!< Invert result; unused for now.
		SelectType selType = SelectType::USER;
		quint32 block = 0;      //!< Block identifier in SelectConfig for SelectType::*_BLOCK
		QString essid;
		QHostAddress ipAddr;
		libnutcommon::MacAddress macAddr;
	};
	QDBusArgument &operator<< (QDBusArgument &argument, const SelectRule &data);
	const QDBusArgument &operator>> (const QDBusArgument &argument, SelectRule &data);

	/** @brief SelectConfig for an environment.
	 *
	 * This structure represents a tree of SelectRules; try qnut for visualization.
	 */
	class SelectConfig {
	public:
		SelectConfig() { }

		QVector<SelectRule> filters;         //!< List of \link SelectRule SelectRules\endlink
		//! List of blocks; each block is a list of filter ids.
		//! The type of the block (AND/OR) is specified in the rule for the block
		QVector< QVector<quint32> > blocks;
	};
	QDBusArgument &operator<< (QDBusArgument &argument, const SelectConfig &data);
	const QDBusArgument &operator>> (const QDBusArgument &argument, SelectConfig &data);

	/** @brief Each EnvironmentConfig of a \link DeviceConfig device\endlink has a list
	 *         of \link IPv4Config interfaces\endlink, which configure the ips.
	 *
	 * And it contains a SelectConfig, which determines which Environment is loaded for a device.
	 */
	class EnvironmentConfig {
	public:
		QString name; //!< A description for that environment. It does not have to be unique.
		std::vector<std::shared_ptr<IPv4Config>> ipv4Interfaces;
		SelectConfig select;

		EnvironmentConfig(const QString &name = "") : name(name) { }
	};
	QDBusArgument &operator<< (QDBusArgument &argument, const EnvironmentConfig &data);
	const QDBusArgument &operator>> (const QDBusArgument &argument, EnvironmentConfig &data);


	/** @brief Selects which method is used to determine the ip address.
	 *
	 * Not all flags can be set:
	 *   assert(USERSTATIC xor (DHCP or (ZEROCONF xor STATIC)))
	 */
	enum class IPv4ConfigFlag : quint32 {
		DHCP      = 1,   //!< Find ip/gateway/dns via DHCP
		ZEROCONF  = 2,   //!< Probe for ip in the IPv4 Link-Local range (RFC 3927).
		STATIC    = 4,   //!< Use values from config file.
		USERSTATIC = 8,  //!< Use values specified at runtime by a user
	};
	using IPv4ConfigFlags = Flags<IPv4ConfigFlag>;
	NUTCOMMON_DECLARE_FLAG_OPERATORS(IPv4ConfigFlags)
	QDBusArgument &operator<< (QDBusArgument &argument, IPv4ConfigFlags flags);
	const QDBusArgument &operator>> (const QDBusArgument &argument, IPv4ConfigFlags &flags);

	/** @brief Each IPv4Config stands for one ip of an interface.
	 *
	 * There are several methods how to to this, and some
	 * additional values: netmask, gateway, dns-servers.
	 *
	 * If the interface has the method IPv4ConfigFlag::STATIC,
	 * the configured values for ip/netmask/... can be queried
	 * with the corresponding functions.
	 */
	class IPv4Config {
	public:
		QHostAddress static_ip;
		QHostAddress static_netmask;
		QHostAddress static_gateway;
		QList<QHostAddress> static_dnsservers;

		IPv4ConfigFlags flags = IPv4ConfigFlag::DHCP | IPv4ConfigFlag::ZEROCONF;
		int gateway_metric = -1;
		int timeout = 0;
		bool continue_dhcp = false;

		IPv4Config() { }
		IPv4Config(IPv4ConfigFlags flags) : flags(flags) { }
	};
	QDBusArgument &operator<< (QDBusArgument &argument, const IPv4Config &data);
	const QDBusArgument &operator>> (const QDBusArgument &argument, IPv4Config &data);

	/** @brief If an interface has to be configured by the user (IPv4ConfigFlag::USERSTATIC), he/she has to
	 *         set that information with this class.
	 *
	 */
	class IPv4UserConfig {
	public:
		QHostAddress ip;
		QHostAddress netmask;
		QHostAddress gateway;
		QList<QHostAddress> dnsservers;

		/** @brief A very basic check if the configuration is valid.
		 */
		bool valid() {
			return !ip.isNull();
		}
	};
	QDBusArgument &operator<< (QDBusArgument &argument, const IPv4UserConfig &data);
	const QDBusArgument &operator>> (const QDBusArgument &argument, IPv4UserConfig &data);
}

Q_DECLARE_METATYPE(libnutcommon::DeviceConfig)
Q_DECLARE_METATYPE(libnutcommon::SelectResult)
Q_DECLARE_METATYPE(QVector< libnutcommon::SelectResult >)
Q_DECLARE_METATYPE(libnutcommon::SelectRule)
Q_DECLARE_METATYPE(libnutcommon::SelectType)
Q_DECLARE_METATYPE(libnutcommon::SelectConfig)
Q_DECLARE_METATYPE(libnutcommon::EnvironmentConfig)
Q_DECLARE_METATYPE(libnutcommon::IPv4ConfigFlags)
Q_DECLARE_METATYPE(libnutcommon::IPv4Config)
Q_DECLARE_METATYPE(libnutcommon::IPv4UserConfig)

#endif
