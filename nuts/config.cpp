
#include "config.h"
#include "configparser_yacc.h"

extern "C" {
#include <stdio.h>
}

void configparserparse(nuts::ConfigParser *cp);
extern FILE *configparserin;

/* sub configuration structures will
 * be deleted by ~Config() !
 */

namespace nuts {
	ConfigParser::ConfigParser(const QString &configFile)
	: m_configFile(configFile), m_config(new nut::Config()) {
		configparserin = fopen(m_configFile.toUtf8().constData(), "r");
		configparserparse(this);
		fclose(configparserin);
	}
	
	ConfigParser::~ConfigParser() {
		delete m_config;
	}
	
	bool ConfigParser::newDevice(const QString &name) {
		if (!m_config->m_devices.contains(name)) {
			m_curdevconfig = new nut::DeviceConfig();
			m_config->m_devices.insert(name, m_curdevconfig);
			m_curdevconfig->m_environments.push_back(new nut::EnvironmentConfig(""));
			return true;
		}
		return false;
	}
	
	bool ConfigParser::devDefaultEnvironment() {
		if (!m_curdevconfig) return false;
		if (!m_curdevconfig->m_environments.count() > 0) return false;
		m_curenvconfig = m_curdevconfig->m_environments[0];
		return true;
	}
	
	bool ConfigParser::devEnvironment(const QString &name) {
		if (!m_curdevconfig) return false;
		m_curenvconfig = new nut::EnvironmentConfig(name);
		m_curdevconfig->m_environments.push_back(m_curenvconfig);
		return true;
	}
	
	bool ConfigParser::envSelect() {
		if (!m_curenvconfig) return false;
		// Only one select block per environment
		if (m_curenvconfig->m_select.filters.count() != 0) return false;
		m_selBlocks.clear();
		return true;
	}
	
	bool ConfigParser::envDHCP() {
		if (!m_curenvconfig) return false;
		if (m_curenvconfig->m_dhcp) return false;
		m_curipv4config = m_curenvconfig->m_dhcp = new nut::IPv4Config();
		m_curenvconfig->m_ipv4Interfaces.push_back(m_curipv4config);
		return true;
	}
	
	bool ConfigParser::envStatic() {
		if (!m_curenvconfig) return false;
		m_curipv4config = new nut::IPv4Config(nut::IPv4Config::DO_STATIC);
		m_curenvconfig->m_ipv4Interfaces.push_back(m_curipv4config);
		return true;
	}
	
	bool ConfigParser::staticIP(const QHostAddress &addr) {
		if (!m_curipv4config) return false;
		if (!m_curipv4config->m_static_ip.isNull()) return false;
		m_curipv4config->m_static_ip = addr;
		return true;
	}
	
	bool ConfigParser::staticNetmak(const QHostAddress &addr) {
		if (!m_curipv4config) return false;
		if (!m_curipv4config->m_static_netmask.isNull()) return false;
		m_curipv4config->m_static_netmask = addr;
		return true;
	}
	
	bool ConfigParser::staticGateway(const QHostAddress &addr) {
		if (!m_curipv4config) return false;
		if (!m_curipv4config->m_static_gateway.isNull()) return false;
		m_curipv4config->m_static_gateway = addr;
		return true;
	}
	
	bool ConfigParser::staticDNS(const QHostAddress &addr) {
		if (!m_curipv4config) return false;
		m_curipv4config->m_static_dnsservers.push_back(addr);
		return true;
	}
	
	void ConfigParser::selectAdd(const nut::SelectRule &rule) {
		quint32 filterid = m_curenvconfig->m_select.filters.count();
		m_curenvconfig->m_select.filters.append(rule);
		m_curenvconfig->m_select.blocks.last().append(filterid);
	}
	
	bool ConfigParser::selectAndBlock() {
		if (!m_curenvconfig) return false;
		quint32 blockid = m_curenvconfig->m_select.blocks.size();
		if (blockid)
			selectAdd(nut::SelectRule(blockid));
		m_curenvconfig->m_select.blocks.append(QVector<quint32>(1, 0));
		return true;
	}
	
	bool ConfigParser::selectOrBlock() {
		if (!m_curenvconfig) return false;
		quint32 blockid = m_curenvconfig->m_select.blocks.size();
		if (blockid)
			selectAdd(nut::SelectRule(blockid));
		m_curenvconfig->m_select.blocks.append(QVector<quint32>(1, 1));
		return true;
	}
	
	bool ConfigParser::selectUser() {
		if (!m_curenvconfig) return false;
		selectAdd(nut::SelectRule());
		return true;
	}
	
	bool ConfigParser::selectARP(const QHostAddress &addr) {
		if (!m_curenvconfig) return false;
		selectAdd(nut::SelectRule(addr));
		return true;
	}
	
	bool ConfigParser::selectARP(const QHostAddress &addr, const nut::MacAddress &mac) {
		if (!m_curenvconfig) return false;
		selectAdd(nut::SelectRule(addr, mac));
		return true;
	}
	
	bool ConfigParser::selectESSID(const QString &essid) {
		if (!m_curenvconfig) return false;
		selectAdd(nut::SelectRule(essid));
		return true;
	}
};
