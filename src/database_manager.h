#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include "local_note.h"

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = 0);
    Q_INVOKABLE void connectToDatabase();
    Q_INVOKABLE int createNote(LocalNote *localNote);
    Q_INVOKABLE void updateNote(LocalNote *localNote);
    Q_INVOKABLE void deleteNoteById(const int id);
    Q_INVOKABLE void deleteAllNotes();
    QList<LocalNote*> retrieveAllNotes();
    LocalNote* retrieveLocalNoteByExternalId(const QString &externalId);

    Q_INVOKABLE QList<QString> retrieveAllTags();
    Q_INVOKABLE int numberOfNotesWithTag(const QString &tagName);

private:
    static const QString DATABASE_NAME;
    static const QString DATABASE_HOSTNAME;
    static const QString TABLE_NAME;
    static const QString TITLE_FIELD;
    static const QString DESCRIPTION_FIELD;
    static const QString PICTURE_PATHS_FIELD;
    static const QString HASHES_FIELD;
    static const QString TAG_EXTERNAL_IDS_FIELD;
    static const QString TAG_NAMES_FIELD;
    static const QString REMINDER_TIMESTAMP_FIELD;
    static const QString EXTERNAL_ID_FIELD;
    static const QString DELETED_FIELD;
    static const QString SYNCHRONIZED_FIELD;

    QSqlDatabase database;

    void openDatabase();
    void createNotesTable();

signals:

public slots:
};

#endif // DATABASEMANAGER_H
