#ifndef QNUT_CONNECTIONMANAGER_H
#define QNUT_CONNECTIONMANAGER_H

#include <QtGui>
#include <QHash>
#include "ui_connman.h"
#include "trayicon.h"
#include "overviewlistmodel.h"
#include "deviceoptions.h"
#include "libnut_cli.h"

namespace qnut {
    using namespace libnut;

    class CConnectionManager : public QMainWindow {
        Q_OBJECT
    private:
        Ui::ConnMan ui;
    public:
        CDeviceManager deviceManager;
        
        QAction * enableDeviceAction;
        QAction * disableDeviceAction;
        QAction * activateEnvironmentAction;
        QAction * deactivateEnvironmentAction;
        QAction * activateInterfaceAction;
        QAction * deactivateInterfaceAction;
        
        QMenu overViewMenu;
        CTrayIcon trayicon;
        COverViewListModel overViewListModel;
        QHash<CDevice *, CDeviceOptions *> deviceOptions;

        CConnectionManager(QWidget * parent = 0);
    public slots:
        void uiAddedDevice(CDevice * dev);
        void uiRemovedDevice(CDevice * dev);
        void uiCurrentTabChanged(int index);
        void uiSelectedDeviceChanged(const QItemSelection & selected, const QItemSelection & deselected);
        void uiShowOverViewPopup(const QPoint & pos);
        //void uiShowEnvironmentsTree();
    };
};

#endif
