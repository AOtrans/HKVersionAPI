#include "mytreeitem.h"

MyTreeItem::MyTreeItem(QString text, QString id, TreeItemType type, void *bindData, bool editAble)
    :QStandardItem(text)
{
    m_itemType = type;
    setEditable(editAble);
    m_mapId = id;
    m_bindData = bindData;
}

TreeItemType MyTreeItem::itemType() const
{
    return m_itemType;
}

void MyTreeItem::setItemType(const TreeItemType &itemType)
{
    m_itemType = itemType;
}

QString MyTreeItem::getMapId() const
{
    return m_mapId;
}

void MyTreeItem::setMapId(const QString &value)
{
    m_mapId = value;
}

void *MyTreeItem::getBindData() const
{
    return m_bindData;
}

void MyTreeItem::setBindData(void *bindData)
{
    m_bindData = bindData;
}
