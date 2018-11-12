#ifndef GIFDIALOG_H
#define GIFDIALOG_H

#include <QDialog>
#include "tree/myrighttreeitem.h"
#include <common.h>

namespace Ui {
class GifDialog;
}

class GifDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GifDialog(QString location, QString time, QString path, QWidget *parent = 0, MyRightTreeItem* trigger = 0);
    ~GifDialog();

private slots:
    void on_pbConfirm_clicked();

private:
    Ui::GifDialog *ui;
    MyRightTreeItem *trigger;
    QString filepath;
};

#endif // GIFDIALOG_H
