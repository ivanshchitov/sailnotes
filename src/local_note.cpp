#include "local_note.h"
#include <QStringList>

/**
 * @brief The constructor.
 */
LocalNote::LocalNote() : LocalNote(QString(), QString(), QList<QString>(), QList<QString>()) { }

/**
 * @brief The constructor.
 * @param title The note title.
 * @param description The note description.
 * @param picturePaths The note picture paths.
 * @param hashes The note picture hashes.
 * @param externalId The note external id.
 * @param deleted True if the note is deleted, false - otherwise.
 * @param synchronized True if the note is synchronized, false - otherwise.
 */
LocalNote::LocalNote(const QString &title, const QString &description, QList<QString> picturePaths,
                     QList<QString> hashes, const QString &externalId, const bool deleted, const bool synchronized)
    : LocalNote(0, title, description, picturePaths, hashes, externalId, deleted, synchronized) { }

/**
 * @brief The constructor.
 * @param id The note local id.
 * @param title The note title.
 * @param description The note description.
 * @param picturePaths The note picture paths.
 * @param hashes The note picture hashes.
 * @param externalId the note external id.
 * @param deleted True if the note is deleted, false - otherwise.
 * @param synchronized True if the note is synchronized, false - otherwise.
 */
LocalNote::LocalNote(const int id, const QString &title, const QString &description, QList<QString> picturePaths,
                     QList<QString> hashes, const QString &externalId, const bool deleted, const bool synchronized)
{
    this->id = id;
    this->title = title;
    this->description = description;
    this->picturePaths = picturePaths;
    this->hashes = hashes;
    this->externalId = externalId;
    this->deleted = deleted;
    this->synchronized = synchronized;
}

/**
 * @brief The constructor.
 * @param id The note local id.
 * @param title The note title.
 * @param description The note description.
 * @param picturePaths The note picture paths as QString with "," as a delimiter.
 * @param hashes The note picture hashes as QString with "," as a delimiter.
 * @param tagExternalIds The note tag external ids as QString with "," as delimiter.
 * @param tagNames The note tag names as QString with "," as delimiter.
 * @param externalId the note external id.
 * @param deleted True if the note is deleted, false - otherwise.
 * @param synchronized True if the note is synchronized, false - otherwise.
 */
LocalNote::LocalNote(const int id, const QString &title, const QString &description, const QString &picturePaths,
                     const QString &hashes, const QString &tagExternalIds, const QString &tagNames, const qint64 reminderTimestamp,
                     const QString &externalId, const bool deleted, const bool synchronized)
{
    this->id = id;
    this->title = title;
    this->description = description;
    addPicturePaths(picturePaths);
    addHashes(hashes);
    setTagsFromStrings(tagNames, tagExternalIds);
    this->reminderTimestamp = reminderTimestamp;
    this->externalId = externalId;
    this->deleted = deleted;
    this->synchronized = synchronized;
}

/**
 * @return The note local id.
 */
int LocalNote::getId() const { return this->id; }

/**
 * @brief Sets the note local id.
 * @param id The note local id.
 */
void LocalNote::setId(const int id) { this->id = id; }

/**
 * @return The note title.
 */
QString LocalNote::getTitle() const { return this->title; }

/**
 * @brief Sets the note title.
 * @param title The new note title.
 */
void LocalNote::setTitle(const QString &title)
{
    this->title = title;
    emit onTitleChanged();
}

/**
 * @return The note description.
 */
QString LocalNote::getDescription() const { return this->description; }

/**
 * @brief Sets the note description.
 * @param description The new note description.
 */
void LocalNote::setDescription(const QString &description)
{
    this->description = description;
    emit onDescriptionChanged();
}

/**
 * @return The note pictures paths as QString with "," as a delimiter.
 */
QString LocalNote::getPicturePathsAsString() const {
    QString paths = QString();
    for (int i = 0; i < picturePaths.size(); i++) {
        paths.append(picturePaths.at(i)).append(i == picturePaths.size() - 1 ? "" : ",");
    }
    return paths;
}

/**
 * @return The list of pictures paths.
 */
QList<QString> LocalNote::getPicturePaths() const { return this->picturePaths; }

/**
 * @brief Sets the note pictures paths as a string.
 * @param picturePaths The note pictures paths.
 */
void LocalNote::addPicturePaths(const QString &picturePaths)
{
    foreach (QString path, picturePaths.split(",")) {
        if (path.length() > 0) this->picturePaths.append(path);
    }
    emit onPicturePathsChanged();
}

/**
 * @brief Sets the note pictures paths.
 * @param picturePaths The list of pictures paths.
 */
void LocalNote::addPicturePaths(QList<QString> picturePaths)
{
    this->picturePaths.append(picturePaths);
}

/**
 * @brief Removes the given picture path from the list.
 * @param picturePath The picture path to remove.
 */
void LocalNote::removePicturePath(const QString &picturePath)
{
    this->picturePaths.removeOne(picturePath);
    emit onPicturePathsChanged();
}

/**
 * @return The note pictures hashes as QString with "," as a delimiter.
 */
QString LocalNote::getHashesAsString() const
{
    QString hashes = QString();
    for (int i = 0; i < this->hashes.size(); i++) {
        hashes.append(this->hashes.at(i)).append(i == this->hashes.size() - 1 ? "" : ",");
    }
    return hashes;
}

/**
 * @return The list of pictures hashes.
 */
QList<QString> LocalNote::getHashes() const
{
    return this->hashes;
}

/**
 * @brief Adds the note pictures hashes to the list.
 * @param hashes The note pictures hashes as a string with the "," delimiter.
 */
void LocalNote::addHashes(const QString &hashes)
{
    foreach (QString hash, hashes.split(",")) {
        if (hash.length() > 0) this->hashes.append(hash);
    }
    emit onHashesChanged();
}

/**
 * @brief Adds the note pictures hashes to the list.
 * @param hashes The note pictures hashes in the list.
 */
void LocalNote::addHashes(QList<QString> hashes)
{
    this->hashes.append(hashes);
}

/**
 * @brief Removes the given picture hash from the list.
 * @param picturePath The picture hash to remove.
 */
void LocalNote::removeHash(const QString &hash)
{
    this->hashes.removeOne(hash);
    emit onHashesChanged();
}

/**
 * @return The tag external ids as QString with "," as a delimiter.
 */
QString LocalNote::getTagExternalIdsAsString() const
{
    QString tagExternalIds = QString();
    for (int i = 0; i < getTagExternalIds().size(); i++) {
        tagExternalIds.append(getTagExternalIds().at(i)).append(i == getTagExternalIds().size() - 1 ? "" : ",");
    }
    return tagExternalIds;
}

/**
 * @return The list of tag external ids.
 */
QList<QString> LocalNote::getTagExternalIds() const
{
    return this->tags.values();
}

/**
 * @return The tag names as QString with "," as a delimiter.
 */
QString LocalNote::getTagNamesAsString() const
{
    QString tagNames = QString();
    for (int i = 0; i < getTagNames().size(); i++) {
        tagNames.append(getTagNames().at(i)).append(i == getTagNames().size() - 1 ? "" : ", ");
    }
    return tagNames;
}

/**
 * @return The list of the note tag names.
 */
QList<QString> LocalNote::getTagNames() const
{
    return this->tags.keys();
}

/**
 * @brief Sets tags from the given string with tags.
 * @param tagNames The tag names as a string with the "," delimiter.
 */
void LocalNote::setTagNamesFromString(const QString &tagNames)
{
    QHashIterator<QString, QString> iterator(this->tags);
    while (iterator.hasNext()) {
        iterator.next();
        if (!tagNames.contains(iterator.key())) this->tags.remove(iterator.key());
    }
    foreach (QString tagName, tagNames.split(QRegExp(",\\s*"))) {
        if (tagName.length() > 0 && !this->tags.contains(tagName)) {
            this->tags[tagName] = "";
        }
    }
    emit onTagNamesChanged();
}

/**
 * @return The hash map with tags.
 */
QHash<QString, QString> LocalNote::getTags()
{
    return this->tags;
}

/**
 * @brief Add the given tag to the hash map.
 * @param tagName The tag name.
 * @param tagExternalId The tag external id.
 */
void LocalNote::addTag(const QString &tagName, const QString &tagExternalId)
{
    this->tags[tagName] = tagExternalId;
}

/**
 * @brief Removes a tag by the given tag external id from the hash map.
 * @param tagExternalId The tag external id to remove.
 */
void LocalNote::removeTagByExternalId(const QString &tagExternalId)
{
    this->tags.remove(this->tags.key(tagExternalId));
}

/**
 * @brief Sets tags from the given strings with the tag names and tag external ids.
 * @param tagNames The tag names as a string with the "," delimiter.
 * @param tagExternalIds The tag external ids as a string with the "," delimiter.
 */
void LocalNote::setTagsFromStrings(const QString &tagNames, const QString &tagExternalIds)
{
    if (tagNames.length() == 0 && tagExternalIds.length() == 0) return;
    QList<QString> tagNamesList = tagNames.split(QRegExp(",\\s*"));
    QList<QString> tagExternalIdsList = tagExternalIds.split(QRegExp(",\\s*"));
    for (int i = 0; i < tagNamesList.size(); i++) {
        this->tags[tagNamesList.at(i)] = tagExternalIdsList.at(i);
    }
}

/**
 * @return The note reminder timestamp.
 */
qint64 LocalNote::getReminderTimestamp() const
{
    return this->reminderTimestamp;
}

/**
 * @brief Sets the note reminder timestamp.
 * @param timestamp The timestamp to set.
 */
void LocalNote::setReminderTimestamp(qint64 timestamp)
{
    this->reminderTimestamp = timestamp;
    emit onReminderTimestampChanged();
}

/**
 * @return The note external id.
 */
QString LocalNote::getExternalId() const { return this->externalId; }

/**
 * @brief Sets the note external id.
 * @param externalId The note external id.
 */
void LocalNote::setExternalId(const QString &externalId) { this->externalId = externalId; }

/**
 * @return True if the note is deleted, false - otherwise.
 */
bool LocalNote::isDeleted() const { return this->deleted; }

/**
 * @brief Sets the note 'deleted' status.
 * @param deleted True if the note is deleted, false - otherwise.
 */
void LocalNote::setDeleted(const bool deleted) { this->deleted = deleted; }

/**
 * @return True if the note is synchronized, false - otherwise.
 */
bool LocalNote::isSynchronized() const { return this->synchronized; }

/**
 * @brief Sets the note 'synchronized' status.
 * @param synchronized True if the note is synchronized, false - otherwise.
 */
void LocalNote::setSynchronized(const bool synchronized)  { this->synchronized = synchronized; }


