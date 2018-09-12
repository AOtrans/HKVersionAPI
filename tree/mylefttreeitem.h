#ifndef MYLEFTTREEITEM_H
#define MYLEFTTREEITEM_H
#include <QStandardItem>
#include <structs/devicedata.h>
#include "common.h"

enum LeftTreeItemType{lDEVICE, lCHANNEL, lROOT, lOTHER};

class MyLeftTreeItem : public QStandardItem
{
public:
    MyLeftTreeItem(QString text, QString m_mapId, LeftTreeItemType type, void* bindData, bool editAble = false);
    LeftTreeItemType itemType() const;
    void setItemType(const LeftTreeItemType &itemType);

    QString getMapId() const;
    void setMapId(const QString &value);

    void *getBindData() const;
    void setBindData(void *bindData);

private:
    LeftTreeItemType m_itemType;
    //use this id to locate target device, case channel it should set to ""
    QString m_mapId;
    void *m_bindData;
};

#endif // MYLEFTTREEITEM_H
