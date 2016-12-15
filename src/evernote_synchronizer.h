#ifndef EVERNOTE_SYNCHRONIZER_H
#define EVERNOTE_SYNCHRONIZER_H

#include <QObject>
#include <QNetworkReply>
#include <QFutureWatcher>
#include "settings.h"
#include "database_manager.h"
#include "note_list_model.h"
#include "file_helper.h"
#include "notification_manager.h"
#include "lib/QEverCloud/QEverCloud/include/QEverCloud.h"

class EvernoteSynchronizer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DatabaseManager* databaseManager READ getDatabaseManager WRITE setDatabaseManager NOTIFY databaseManagerChanged)
    Q_PROPERTY(FileHelper* fileHelper READ getFileHelper WRITE setFileHelper NOTIFY fileHelperChanged)
    Q_PROPERTY(NoteListModel* noteListModel READ getNoteListModel WRITE setNoteListModel NOTIFY noteListModelChanged)
    Q_PROPERTY(NotificationManager* notificationManager READ getNotificationManager WRITE setNotificationManager NOTIFY notificationManagerChanged)
    Q_PROPERTY(Settings* settings READ getSettings WRITE setSettings NOTIFY settingsChanged)
public:
    explicit EvernoteSynchronizer();
    Q_INVOKABLE QString createAuthenticationUrl();
    Q_INVOKABLE void authenticate(QString url);
    Q_INVOKABLE void startSynchronization();

    DatabaseManager *getDatabaseManager() const;
    void setDatabaseManager(DatabaseManager *databaseManager);

    FileHelper *getFileHelper() const;
    void setFileHelper(FileHelper *fileHelper);

    NoteListModel *getNoteListModel() const;
    void setNoteListModel(NoteListModel *noteListModel);

    NotificationManager *getNotificationManager() const;
    void setNotificationManager(NotificationManager *notificationManager);

    Settings *getSettings() const;
    void setSettings(Settings *settings);

private:
    static const QString HOST_NAME;
    static const QString CONSUMER_KEY;
    static const QString CONSUMER_SECRET;
    static const QString NOTE_DESCRIPTION_TEMPLATE;
    QString oauthUrl;
    Settings *settings;
    QFutureWatcher<void> watcher;
    DatabaseManager *databaseManager;
    FileHelper *fileHelper;
    NoteListModel *noteListModel;
    NotificationManager *notificationManager;

    QString requestGet(QString url);
    QList<Note> retrieveNotes(NoteStore *noteStore);
    void synchronize();
    void loadNotesFromService(NoteStore *noteStore);
    void sendNotesToService(NoteStore *noteStore);
    Note createNoteByLocalNote(NoteStore *noteStore, LocalNote *localNote);
    QString createNoteContent(QString localNoteDescription, QList<Resource> resources = QList<Resource>());
    void createLocalTagsOnService(NoteStore *noteStore, LocalNote *localNote);
    QList<Resource> createNoteResources(LocalNote *localNote);
    QHash<QString, QString> loadNotePicturesFromService(NoteStore *noteStore, Note note, const QString &noteContent, QList<QString> localHashes);
    QHash<QString, QString> loadNoteTagsFromService(NoteStore *noteStore, Note note, QList<QString> localTagNames);
    void saveServiceNoteToLocalNote(NoteStore *noteStore, Note note, LocalNote *localNote);
    void setupNotification(LocalNote *localNote);

signals:
    void databaseManagerChanged();
    void fileHelperChanged();
    void noteListModelChanged();
    void notificationManagerChanged();
    void settingsChanged();
private slots:
    void updateNoteList();
};

#endif // EVERNOTE_SYNCHRONIZER_H
