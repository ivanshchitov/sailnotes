#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include "lib/QEverCloud/QEverCloud/include/QEverCloud.h"

using namespace qevercloud;

class Settings : public QObject
{
    Q_OBJECT
public:
    static const QString AUTH_TOKEN_KEY;
    static const QString NOTE_STORE_URL_KEY;
    static const QString LAST_UPDATE_COUNT_KEY;
    static const QString LAST_SYNC_TIME;

    explicit Settings(QObject *parent = 0);
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value = QVariant());
    Q_INVOKABLE QVariant value(const QString &key);
    Q_INVOKABLE QString valueToString(const QString &key);
    Q_INVOKABLE Timestamp valueToTimestamp(const QString &key);
    Q_INVOKABLE int valueToInt(const QString &key);
    Q_INVOKABLE bool contains(const QString &key);

private:
    QSettings settings;

signals:

public slots:
};

#endif // SETTINGS_H
