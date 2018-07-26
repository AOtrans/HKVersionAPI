#ifndef MYTREEITEM_H
#define MYTREEITEM_H
#include <QStandardItem>
#include <structs/devicedata.h>

enum TreeItemType{DEVICE, CHANNEL, ROOT, OTHER};

class MyTreeItem : public QStandardItem
{
public:
    MyTreeItem(QString text, QString m_mapId, TreeItemType type, void* bindData, bool editAble = false);
    TreeItemType itemType() const;
    void setItemType(const TreeItemType &itemType);

    QString getMapId() const;
    void setMapId(const QString &value);

    void *getBindData() const;
    void setBindData(void *bindData);

private:
    TreeItemType m_itemType;
    QString m_mapId;
    void *m_bindData;
};

#endif // MYTREEITEM_H
