#include "mylefttreeitem.h"
extern Config* config;
MyLeftTreeItem::MyLeftTreeItem(QString text, QString id, LeftTreeItemType type, void *bindData, bool editAble)
    :QStandardItem(text)
{
    m_itemType = type;
    setEditable(editAble);
    m_mapId = id;
    m_bindData = bindData;

    if(type == lDEVICE)
    {
        this->setIcon(QIcon(config->DEVICE_ICON));
    }
    else if(type == lCHANNEL)
    {
        this->setIcon(QIcon(config->CAMERAL_ICON));
    }
}

LeftTreeItemType MyLeftTreeItem::itemType() const
{
    return m_itemType;
}

void MyLeftTreeItem::setItemType(const LeftTreeItemType &itemType)
{
    m_itemType = itemType;
}

QString MyLeftTreeItem::getMapId() const
{
    return m_mapId;
}

void MyLeftTreeItem::setMapId(const QString &value)
{
    m_mapId = value;
}

void *MyLeftTreeItem::getBindData() const
{
    return m_bindData;
}

void MyLeftTreeItem::setBindData(void *bindData)
{
    m_bindData = bindData;
}
