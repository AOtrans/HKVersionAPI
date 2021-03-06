#include "myrighttreeitem.h"

MyRightTreeItem::MyRightTreeItem(QString text, RightTreeItemType type, QString bindData, bool editAble)
    :QStandardItem(text)
{
    m_itemType = type;
    setEditable(editAble);
    m_bindData = bindData;


    if(type == rDEVICE)
    {
        this->setIcon(QIcon(DEVICE_ICON));
    }
    else if(type == rCHANNEL)
    {
        this->setIcon(QIcon(CAMERAL_ICON));
    }
    else if(type == rGIF1)
    {
        this->setIcon(QIcon(WARNING_ICON_1));
    }
    else if(type == rGIF2)
    {
        this->setIcon(QIcon(WARNING_ICON_2));
    }
    else if(type == rDATE)
    {
        this->setIcon(QIcon(DATE_ICON));
    }
}

RightTreeItemType MyRightTreeItem::itemType() const
{
    return m_itemType;
}

void MyRightTreeItem::setItemType(const RightTreeItemType &itemType)
{
    m_itemType = itemType;
}

QString MyRightTreeItem::bindData() const
{
    return m_bindData;
}

void MyRightTreeItem::setBindData(QString bindData)
{
    m_bindData = bindData;
}
