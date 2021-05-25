#pragma once
#include <QTreeWidget>
#include <opcuaclient/UaBrowser.h>

#include "util.h"

ELESYOPCUACLIENT_START

class QUaBrowser : public QTreeWidget
{
Q_OBJECT
public:
    using Ptr = std::shared_ptr<QUaBrowser>;
    QUaBrowser(QWidget *parent = nullptr);

    void setConnection(UaConnection::Ptr conn);

    QList<QTreeWidgetItem*> readCurrentDir(const std::string &nodeId,
                                           BrowseDirection direction = BrowseDirection::forward,
                                           ReferenceCategory refCategory = ReferenceCategory::hierarchical);

    inline UaStatusCode lastError() const { return status; }
    inline bool isValid() const { return status == uaGood; }
    inline bool isConnected() const;
    void checkConnected();
    bool browserInit();

private:
    QTreeWidgetItem* createItem(const QString &text, const BrowseResult &browseResult);

    UaConnection::Ptr connection;
    UaBrowser::Ptr    browser;
    UaStatusCode      status;
    QIcon             varIcon { QPixmap("../data.png") };

private slots:
    void directoryOpened(QTreeWidgetItem* current, QTreeWidgetItem* previous);
};

ELESYOPCUACLIENT_END

