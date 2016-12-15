#include "settings.h"

const QString Settings::AUTH_TOKEN_KEY = "auth_token";
const QString Settings::NOTE_STORE_URL_KEY = "note_store_url";
const QString Settings::LAST_UPDATE_COUNT_KEY = "last_update_count";
const QString Settings::LAST_SYNC_TIME = "last_sync_time";

/**
 * @brief The constructor.
 * @param parent The QObject parent instance.
 */
Settings::Settings(QObject *parent) : QObject(parent)
{

}

/**
 * @brief Sets the given setting value by the given setting key.
 * @param key The setting key.
 * @param value The setting value.
 */
void Settings::setValue(const QString &key, const QVariant &value)
{
    settings.setValue(key, value);
}

/**
 * @brief Returns the setting value by the given key.
 * @param key The setting key.
 * @return The setting value as QVariant.
 */
QVariant Settings::value(const QString &key)
{
    return settings.value(key);
}

/**
 * @brief Returns the string setting value by the given key.
 * @param key The setting key.
 * @return The setting value as QString.
 */
QString Settings::valueToString(const QString &key)
{
    return settings.value(key).toString();
}

/**
 * @brief Returns the Timestamp setting value by the given key.
 * @param key The setting key
 * @return The setting value as Timestamp.
 */
Timestamp Settings::valueToTimestamp(const QString &key)
{
    return settings.value(key).toLongLong();
}

/**
 * @brief Returns the integer setting value by the given key.
 * @param key The setting key
 * @return The setting value as int.
 */
int Settings::valueToInt(const QString &key)
{
    return settings.value(key).toInt();
}

/**
 * @brief Check that a setting with the given key is already contained.
 * @param key The setting key.
 * @return True if the settings store already contains a setting with the given key,
 *         false - otherwise.
 */
bool Settings::contains(const QString &key)
{
    return settings.contains(key);
}

