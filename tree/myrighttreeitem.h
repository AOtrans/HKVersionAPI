#ifndef MYRIGHTTREEITEM_H
#define MYRIGHTTREEITEM_H
#include <QStandardItem>
#include "common.h"

enum RightTreeItemType{rROOT, rDATE, rDEVICE, rCHANNEL, rGIF1, rGIF2, rGIF3, rOTHER};

class MyRightTreeItem : public QStandardItem
{
public:
    MyRightTreeItem(QString text, RightTreeItemType type, QString bindData = "", bool editAble = false);
    RightTreeItemType itemType() const;
    void setItemType(const RightTreeItemType &itemType);

    QString bindData() const;
    void setBindData(QString bindData);

private:
    RightTreeItemType m_itemType;
    QString m_bindData;
};

#endif // MYRIGHTTREEITEM_H
