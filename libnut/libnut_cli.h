#ifndef LIBNUT_LIBNUT_CLI_H
#define LIBNUT_LIBNUT_CLI_H

#include <QObject>
#include <QList>
#include <QHostAddress>
#include "libnut_types.h"
#include "libnut_server_proxy.h"
#include "libnut_exceptions.h"
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QDBusObjectPath>
#include <QFile>
#include <QTextStream>
#include <QHash>

namespace libnut {
    class CDeviceManager;

    class CLog;
    class CDevice;
    class CEnvironment;
    class CInterface;

    typedef QList<CDevice *> CDeviceList;
    typedef QList<CEnvironment *> CEnvironmentList;
    typedef QList<CInterface *> CInterfaceList;
};

namespace libnut {

    class CLog : public QObject {
        Q_OBJECT
    private:
        QFile file;
        QTextStream outStream;
        bool fileLoggingEnabled;
    public:
        CLog(QObject * parent, QString fileName);
        inline QFile::FileError error() const {
            return file.error();
        }
        inline bool getFileLoggingEnabled() const {
            return fileLoggingEnabled;
        }
        inline void setFileLoggingEnabled(bool isEnabled) {
            fileLoggingEnabled = isEnabled && (file.error() == QFile::NoError);
        }
        void operator<<(QString text);
    signals:
        void printed(const QString & line);
    };
    class QDBusObjectPathList : public QList<QDBusObjectPath> {
        public:
            int removeAll(const QDBusObjectPath &path);
            /*bool operator!= ( const QList<QDBusObjectPath> & other ) const;
            QList<QDBusObjectPath> operator+ ( const QList<QDBusObjectPath> & other ) const;
            QList<QDBusObjectPath> & operator+= ( const QList<QDBusObjectPath> & other );
            QList<QDBusObjectPath> & operator+= ( const QDBusObjectPath & value );
            QList<QDBusObjectPath> & operator<< ( const QList<QDBusObjectPath> & other );
            QList<QDBusObjectPath> & operator<< ( const QDBusObjectPath & value );
            */
            QDBusObjectPathList & operator= ( const QDBusObjectPathList & other );
            QDBusObjectPathList & operator= ( const QList<QDBusObjectPath> & other );
            /*bool operator== ( const QList<QDBusObjectPath> & other ) const;
            QDBusObjectPath & operator[] ( int i );
            const QDBusObjectPath & operator[] ( int i ) const;
            */
    };

    class CLibNut : public QObject {
        Q_OBJECT
        protected:
            QDBusConnectionInterface * dbusConnectionInterface;
            QDBusConnection * dbusConnection;
            void serviceCheck(QDBusConnectionInterface * interface);
            void objectCheck(QDBusConnectionInterface * interface);
        public:
            CLibNut(QObject * parent) : QObject(parent) {}

    };

    class CDeviceManager : public CLibNut {
        Q_OBJECT
        friend class CDevice;
        friend class CEnvironment;
        friend class CInterface;
        friend class DBusDeviceManagerInterface;
    private:
        DBusDeviceManagerInterface * dbusDevmgr;
        QHash<QDBusObjectPath, CDevice* > dbusDevices;
        QDBusConnection dbusConnection;
        CLog * log;
        void rebuild(QList<QDBusObjectPath> paths);
    private slots:
        void dbusDeviceAdded(const QDBusObjectPath &objectpath);
        void dbusDeviceRemoved(const QDBusObjectPath &objectpath);
    public:
        CDeviceList devices;
        void init(CLog * inlog);

        CDeviceManager(QObject * parent);
        ~CDeviceManager();
    public slots:
        void refreshAll();
    signals:
        void deviceAdded(CDevice * device);
        void deviceRemoved(CDevice * device); //nach entfernen aus der liste aber vor dem löschen
    };

    class CDevice : public CLibNut {
        Q_OBJECT
        friend class CDeviceManager;
        friend class CEnvironment;
        friend class CInterface;
        friend class DBusDeviceInterface;
    private:
        CDeviceManager * parent;
        QDBusObjectPath dbusPath;
        QDBusObjectPath dbusActiveEnvironment;
        QHash<QDBusObjectPath, CEnvironment*> dbusEnvironments;
        CLog * log;


        DBusDeviceInterface * dbusDevice;
        void refreshAll();
        void setActiveEnvironment(CEnvironment * env, QDBusObjectPath dbusPath);
        void rebuild(QList<QDBusObjectPath> paths);

    private slots:
        void environmentChangedActive(const QDBusObjectPath &newenv);
        void environmentAdded(const QDBusObjectPath &path);
        void environmentRemoved(const QDBusObjectPath &path);
        void stateChanged(const bool &state);
    public:
        CEnvironmentList environments;
        
        QString name;
        bool enabled;
        int type;
        CEnvironment * activeEnvironment;
        
        CDevice(CDeviceManager * parent, QDBusObjectPath dbuspath);
        ~CDevice();

    public slots:
        void enable();
        void disable();
        void addEnvironment(QString name);
        void removeEnvironment(CEnvironment * environment); //only user defineable
        void setEnvironment(CEnvironment * environment);
        
    signals:
        void environmentChangedActive(CEnvironment * current, CEnvironment * previous);
        void environmentsUpdated();
        void environmentAdded(CEnvironment * environment);
        void environmentRemoved(CEnvironment * environment);
        void enabledChanged(bool enabed);
    };
    
    class CEnvironment : public CLibNut {
        Q_OBJECT
        friend class CDeviceManager;
        friend class CDevice;
        friend class CInterface;
        friend class DBusEnvironmentInterface;
    private:
        CDevice * parent;
        QDBusObjectPath dbusPath;
        CLog * log;
        QHash<QDBusObjectPath, CInterface *> dbusInterfaces;
        DBusEnvironmentInterface * dbusEnvironment;
        
        void refreshAll();
        void rebuild(const QList<QDBusObjectPath> &paths);
    private slots:
        void dbusinterfaceAdded(const QDBusObjectPath &path);
        void dbusinterfaceRemoved(const QDBusObjectPath &path);
        void dbusstateChanged(const bool &state);
    public:
        bool active;
        QString name;
        QList<libnut_SelectConfig> selectStatements;
        CInterfaceList interfaces;
        
        CEnvironment(CDevice * parent, QDBusObjectPath dbusPath);
        ~CEnvironment();
    public slots:
        void enter();
        void addInterface(bool isStatic, QHostAddress ip, QHostAddress netmask, QHostAddress gateway, bool active);
        void removeInterface(CInterface * interface);
        
    signals:
        void activeChanged(bool active);
        void interfacesUpdated();
        void interfaceAdded(CInterface * interface);
        void interfaceRemoved(CInterface * interface);
    };
    
    class CInterface : public CLibNut {
        Q_OBJECT
        friend class CDeviceManager;
        friend class CDevice;
        friend class CEnvironment;
        friend class DBusInterfaceInterface;
    private:
        CEnvironment * parent;
        QDBusObjectPath dbusPath;
        CLog * log;
        DBusInterfaceInterface * dbusInterface;
        void refreshAll();
    private slots:
        void dbusstateChanged(const libnut_InterfaceProperties &properties);
    public:
        bool isStatic;
        bool active;
        bool userDefineable;
        QHostAddress ip;
        QHostAddress netmask;
        QHostAddress gateway;

        CInterface(CEnvironment * parent, QDBusObjectPath dbusPath);
        ~CInterface();
    public slots:
        void activate();
        void deactivate();
        void setIP(QHostAddress & address); //zuvor pointer
        void setNetmask(QHostAddress & address); //zuvor pointer
        void setGateway(QHostAddress & address); //zuvor pointer
        void setDynamic(); // war zuvor nicht da
        inline void setStatic(bool state) {} //wird entfernt
        
    signals:
        void activeChanged(bool active); //zuvor activeStateChanged()
        void ipconfigChanged(bool isStatic, QHostAddress ip, QHostAddress netmask, QHostAddress gateway);
    };
};

#endif
