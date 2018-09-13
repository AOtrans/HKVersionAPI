#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QThread>
#include "structs/channeldata.h"
#include "tools/commonfuncs.h"
#include "structs/commonstructs.h"
class ChannelData;

class SenderThread : public QThread
{
    Q_OBJECT
public:
    SenderThread(ChannelData *cdata, QObject *parent);
    virtual ~SenderThread() override;
    virtual void run() override;

private:
    ChannelData *cdata = NULL;

signals:
    void addRow(QStringList);
};

#endif // SENDERTHREAD_H
