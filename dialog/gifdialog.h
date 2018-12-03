#ifndef GIFDIALOG_H
#define GIFDIALOG_H

#include <QDialog>
#include "tree/myrighttreeitem.h"
#include <common.h>
#include <QMovie>
#include <QTreeView>

namespace Ui {
class GifDialog;
}

class GifDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GifDialog(QString location, QString time, QString path, QWidget *parent = 0, MyRightTreeItem* trigger = 0, QTreeView* view = 0);
    ~GifDialog();

    void reinit(QString location, QString time, QString path, MyRightTreeItem* trigger);

private slots:
    void on_pbConfirm_clicked();

    void on_pbNext_clicked();

    void on_pbPre_clicked();

private:
    Ui::GifDialog *ui;
    MyRightTreeItem *trigger;
    QString filepath;
    QMovie *m;
    QTreeView* view;
};

#endif // GIFDIALOG_H
