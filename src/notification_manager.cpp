#include "notification_manager.h"
#include <QDebug>

/**
 * @brief The constructor.
 * @param parent The QObject parent instance.
 */
NotificationManager::NotificationManager(QObject *parent) : QObject(parent)
{

}

/**
 * @brief Publishes a notification with the given summary and body at the given datetime.
 * @param noteId The note record local id to save a timer.
 * @param summary The notification summary.
 * @param body The notification body.
 * @param dateTime The notification datetime.
 */
void NotificationManager::publishNotification(const int noteId, const QString &summary,
                                              QString body, QDateTime dateTime)
{
    QTimer *timer = new QTimer();
    timer->setTimerType(Qt::PreciseTimer);
    body.remove(QRegExp("<[^>]*>")).remove("\n");
    connect(timer, &QTimer::timeout, [summary, body, timer](){
        Notification notification;
        notification.setSummary(summary);
        notification.setBody(body);
        notification.setPreviewSummary(summary);
        notification.setPreviewBody(body);
        notification.publish();
        timer->stop();
    });
    if (this->timers.contains(noteId)) removeNotification(noteId);
    timer->start(QDateTime::currentDateTime().secsTo(dateTime) * 1000);
    this->timers[noteId] = timer;
}

/**
 * @brief Removes a notification by the given note local id.
 * @param noteId The note record local id to remove a timer.
 */
void NotificationManager::removeNotification(const int noteId)
{
    if (this->timers.contains(noteId)) {
        this->timers.value(noteId)->stop();
        this->timers.remove(noteId);
    }
}


