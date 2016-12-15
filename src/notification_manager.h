#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QHash>
#include <QTimer>
#include <nemonotifications-qt5/notification.h>

class NotificationManager : public QObject
{
    Q_OBJECT
public:
    explicit NotificationManager(QObject *parent = 0);
    Q_INVOKABLE void publishNotification(const int noteId, const QString &summary,
                                         QString body, QDateTime dateTime);
    Q_INVOKABLE void removeNotification(const int noteId);

private:
    QHash<int, QTimer*> timers;

signals:

public slots:
};

#endif // NOTIFICATIONMANAGER_H
