#include "database_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlResult>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

const QString DatabaseManager::DATABASE_NAME = "sailnotes_database.sqlite";
const QString DatabaseManager::DATABASE_HOSTNAME = "sailnotes";
const QString DatabaseManager::TABLE_NAME = "notes";
const QString DatabaseManager::TITLE_FIELD = "title";
const QString DatabaseManager::DESCRIPTION_FIELD = "description";
const QString DatabaseManager::PICTURE_PATHS_FIELD = "picturePaths";
const QString DatabaseManager::HASHES_FIELD = "hashes";
const QString DatabaseManager::TAG_EXTERNAL_IDS_FIELD = "tagExternalIds";
const QString DatabaseManager::TAG_NAMES_FIELD = "tagNames";
const QString DatabaseManager::REMINDER_TIMESTAMP_FIELD = "reminderTimestamp";
const QString DatabaseManager::EXTERNAL_ID_FIELD = "externalId";
const QString DatabaseManager::DELETED_FIELD = "deleted";
const QString DatabaseManager::SYNCHRONIZED_FIELD = "synchronized";

/**
 * @brief The constructor.
 * @param parent The parent QObject instance.
 */
DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
    connectToDatabase();
}

/**
 * @brief Makes a connect to the local SQLite database.
 */
void DatabaseManager::connectToDatabase()
{
    openDatabase();
    createNotesTable();
}

/**
 * @brief Creates a new note in the database.
 * @param localNote The new local note.
 * @return The inserted note id.
 */
int DatabaseManager::createNote(LocalNote *localNote)
{
    QSqlQuery query;
    query.prepare("INSERT INTO notes (title, description, picturePaths, hashes, tagExternalIds, tagNames, reminderTimestamp, externalId, synchronized) "
                  "VALUES (:title, :description, :picturePaths, :hashes, :tagExternalIds, :tagNames, :reminderTimestamp, :externalId, :synchronized);");
    query.bindValue(":title", localNote->getTitle());
    query.bindValue(":description", localNote->getDescription());
    query.bindValue(":picturePaths", localNote->getPicturePathsAsString());
    query.bindValue(":hashes", localNote->getHashesAsString());
    query.bindValue(":tagExternalIds", localNote->getTagExternalIdsAsString());
    query.bindValue(":tagNames", localNote->getTagNamesAsString());
    query.bindValue(":reminderTimestamp", localNote->getReminderTimestamp());
    query.bindValue(":externalId", localNote->getExternalId());
    query.bindValue(":synchronized", localNote->isSynchronized());
    if (!query.exec()) {
        qDebug() << query.lastQuery();
        qDebug() << "QDEBUG: New note isn't inserted: " + query.lastError().text();
    }
    return query.lastInsertId().toInt();
}

/**
 * @brief Updates the fiven local note in the database.
 * @param localNote The local note to update.
 */
void DatabaseManager::updateNote(LocalNote *localNote)
{
    QSqlQuery query;
    query.prepare("UPDATE notes SET title = :title, description = :description, picturePaths = :picturePaths, "
                  "hashes = :hashes, tagExternalIds = :tagExternalIds, tagNames = :tagNames, "
                  "reminderTimestamp = :reminderTimestamp, externalId = :externalId, "
                  "synchronized = :synchronized WHERE id = :id;");
    query.bindValue(":title", localNote->getTitle());
    query.bindValue(":description", localNote->getDescription());
    query.bindValue(":picturePaths", localNote->getPicturePathsAsString());
    query.bindValue(":hashes", localNote->getHashesAsString());
    query.bindValue(":tagExternalIds", localNote->getTagExternalIdsAsString());
    query.bindValue(":tagNames", localNote->getTagNamesAsString());
    query.bindValue(":reminderTimestamp", localNote->getReminderTimestamp());
    query.bindValue(":externalId", localNote->getExternalId());
    query.bindValue(":synchronized", localNote->isSynchronized());
    query.bindValue(":id", localNote->getId());
    if (!query.exec()) {
        qDebug() << query.lastQuery();
        qDebug() << "QDEBUG: The note isn't updated: " + query.lastError().text();
    }
}

/**
 * @brief Deletes a note from the database by the given id.
 *
 * The method deletes a record by the given id the record has no valid external id field
 * and marks the record as 'deleted' and not 'synchronized' if the record has the external id field.
 *
 * @param id The note record id.
 */
void DatabaseManager::deleteNoteById(const int id)
{
    QSqlQuery query;
    query.exec(QStringLiteral("SELECT * FROM %1 WHERE id = %2;").arg(TABLE_NAME).arg(id));
    query.next();
    if (query.value(EXTERNAL_ID_FIELD).toString().isEmpty()) {
        query.exec(QStringLiteral("DELETE FROM %1 WHERE id = %2;").arg(TABLE_NAME).arg(id));
    } else {
        query.exec(QStringLiteral("UPDATE %1 SET %2 = %4, %3 = %5 WHERE id = %6;")
                   .arg(TABLE_NAME).arg(DELETED_FIELD).arg(SYNCHRONIZED_FIELD).arg(1).arg(0).arg(id));
    }
}

/**
 * @brief Deletes all notes from the table.
 */
void DatabaseManager::deleteAllNotes()
{
    QSqlQuery query;
    query.exec(QStringLiteral("DELETE FROM %1").arg(TABLE_NAME));
}

/**
 * @brief Retrieve all notes from the database;
 * @return The list of the LocalNotes objects.
 */
QList<LocalNote*> DatabaseManager::retrieveAllNotes()
{
    QSqlQuery query;
    QList<LocalNote*> localNotes;
    query.exec(QStringLiteral("SELECT * FROM %1;").arg(TABLE_NAME));
    while (query.next()) {
        localNotes.append(new LocalNote(query.value("id").toInt(),
                                        query.value(TITLE_FIELD).toString(),
                                        query.value(DESCRIPTION_FIELD).toString(),
                                        query.value(PICTURE_PATHS_FIELD).toString(),
                                        query.value(HASHES_FIELD).toString(),
                                        query.value(TAG_EXTERNAL_IDS_FIELD).toString(),
                                        query.value(TAG_NAMES_FIELD).toString(),
                                        query.value(REMINDER_TIMESTAMP_FIELD).toLongLong(),
                                        query.value(EXTERNAL_ID_FIELD).toString(),
                                        query.value(DELETED_FIELD).toBool(),
                                        query.value(SYNCHRONIZED_FIELD).toBool()));
    }
    return localNotes;
}

/**
 * @brief Retrieves the local note by the given external id.
 * @param externalId The note external id.
 * @return The local note with the given external id.
 */
LocalNote* DatabaseManager::retrieveLocalNoteByExternalId(const QString &externalId)
{
    QSqlQuery query;
    query.exec(QStringLiteral("SELECT * FROM %1 WHERE %2 = '%3';")
               .arg(TABLE_NAME).arg(EXTERNAL_ID_FIELD).arg(externalId));
    if (query.next()) {
        return new LocalNote(query.value("id").toInt(),
                             query.value(TITLE_FIELD).toString(),
                             query.value(DESCRIPTION_FIELD).toString(),
                             query.value(PICTURE_PATHS_FIELD).toString(),
                             query.value(HASHES_FIELD).toString(),
                             query.value(TAG_EXTERNAL_IDS_FIELD).toString(),
                             query.value(TAG_NAMES_FIELD).toString(),
                             query.value(REMINDER_TIMESTAMP_FIELD).toLongLong(),
                             query.value(EXTERNAL_ID_FIELD).toString(),
                             query.value(DELETED_FIELD).toBool(),
                             query.value(SYNCHRONIZED_FIELD).toBool());
    } else {
        return NULL;
    }
}

/**
 * @brief Retrieves all tag names from the database.
 * @return  The list of unique tag names.
 */
QList<QString> DatabaseManager::retrieveAllTags()
{
    QStringList tagNames;
    QSqlQuery query;
    query.exec(QStringLiteral("SELECT tagNames FROM %1 WHERE %2 = 0").arg(TABLE_NAME).arg(DELETED_FIELD));
    while (query.next()) {
        foreach (QString tagName, query.value(TAG_NAMES_FIELD).toString().split(QRegExp(",\\s*"))) {
            if (tagName.length() > 0) tagNames.append(tagName);
        }
    }
    return QSet<QString>::fromList(tagNames).toList();
}

/**
 * @brief Retrieves the number of notes with the given tag.
 * @param tag The tag name.
 * @return The number of notes with the given tag.
 */
int DatabaseManager::numberOfNotesWithTag(const QString &tag)
{
    QSqlQuery query;
    query.exec(QStringLiteral("SELECT COUNT(*) FROM %1 WHERE %2 = 0 AND %3 LIKE '%%4%'")
               .arg(TABLE_NAME).arg(DELETED_FIELD).arg(TAG_NAMES_FIELD).arg(tag));
    query.next();
    return query.value(0).toInt();
}

/**
 * @brief Opens the local database.
 */
void DatabaseManager::openDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setHostName(DATABASE_HOSTNAME);
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if (!dir.exists()) dir.mkpath(".");
    database.setDatabaseName(dir.path() + "/" + DATABASE_NAME);
    if (!database.open()) {
        qDebug() << "QDEBUG: Database is not opened: " + database.lastError().text();
    }
}

/**
 * @brief Creates the 'notes' table in the database if this table isn't exists already.
 */
void DatabaseManager::createNotesTable()
{
    QSqlQuery query;
    query.prepare(QStringLiteral("CREATE TABLE IF NOT EXISTS %1 ("
                                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "%2 TEXT, %3 TEXT, %4 TEXT, %5 TEXT, %6 TEXT, "
                                 "%7 TEXT, %8 INTEGER DEFAULT 0, %9 TEXT, "
                                 "%10 INTEGER NOT NULL DEFAULT 0, %11 INTEGER NOT NULL DEFAULT 0);")
                  .arg(TABLE_NAME).arg(TITLE_FIELD).arg(DESCRIPTION_FIELD).arg(PICTURE_PATHS_FIELD)
                  .arg(HASHES_FIELD).arg(TAG_EXTERNAL_IDS_FIELD).arg(TAG_NAMES_FIELD).arg(REMINDER_TIMESTAMP_FIELD)
                  .arg(EXTERNAL_ID_FIELD).arg(DELETED_FIELD).arg(SYNCHRONIZED_FIELD));
    if (!query.exec()) {
        qDebug() << "QDEBUG: Table is not created: " + query.lastError().text();
    }
}
