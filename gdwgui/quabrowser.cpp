#include "quabrowser.h"
#include <opcuaclient/UaBrowser.h>

ELESYOPCUACLIENT_START

QUaBrowser::QUaBrowser(QWidget *parent)
    : QTreeWidget(parent)
{
    setDragEnabled(true);
    setHeaderLabel("nodeId");
    QObject::connect(this, &QTreeWidget::currentItemChanged, this, &QUaBrowser::directoryOpened);
}

void QUaBrowser::setConnection(UaConnection::Ptr conn)
{
    connection = conn;
    if(!isConnected()) {
        status = uaBad;
    }
    else if(browserInit()) {
        clear();
        auto root = new QTreeWidgetItem({NODEID_OBJECTS});
        addTopLevelItem(root);
        root->addChildren(readCurrentDir(NODEID_OBJECTS));
    }
}

QList<QTreeWidgetItem*> QUaBrowser::readCurrentDir(const std::string& nodeId, BrowseDirection direction, ReferenceCategory refCategory)
{
    browser->changeBrowsePosition(nodeId);
    std::vector<std::string> tmp;
    BrowseResults result;
    status = browser->browse(direction, refCategory, tmp, result);
    QList<QTreeWidgetItem*> items;
    for(size_t i = 0; i < tmp.size(); ++i) {
        auto currentNodeId = tmp[i];
        auto newChild = createItem(QString::fromStdString(currentNodeId), result[i]);
        items.append(newChild);
    }
    return items;
}

bool QUaBrowser::isConnected() const
{
    return connection->isConnected();
}

bool QUaBrowser::browserInit()
{
    status = createBrowser(connection, browser);
    return status == uaGood;
}

QTreeWidgetItem *QUaBrowser::createItem(const QString &text, const BrowseResult &browseResult)
{
    auto newChild = new QTreeWidgetItem;
    newChild->setText(0, text);
    if(browseResult.nodeClass == UaNodeClass::cVariable) {
        newChild->setIcon(0, varIcon);
    }
    else {
        newChild->setFlags(newChild->flags() ^ Qt::ItemIsDragEnabled);
    }
    return newChild;
}

void QUaBrowser::directoryOpened(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if(current->childCount() == 0) {
        current->addChildren(readCurrentDir(current->text(0).toStdString()));
    }
}
ELESYOPCUACLIENT_END
