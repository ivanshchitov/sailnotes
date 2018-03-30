#include "evernote_synchronizer.h"
#include <QEventLoop>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QDomDocument>
#include <QDomNodeList>
#include <QMimeDatabase>

using namespace qevercloud;

//#error You should use the registered Evernote API Key: "https://dev.evernote.com/doc/"
const QString EvernoteSynchronizer::CONSUMER_KEY = "consumerKey";
const QString EvernoteSynchronizer::CONSUMER_SECRET = "consumerSecret";

const QString EvernoteSynchronizer::HOST_NAME = "sandbox.evernote.com";
const QString EvernoteSynchronizer::NOTE_DESCRIPTION_TEMPLATE = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                                                "<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">"
                                                                "<en-note>%1</en-note>";

/**
 * @brief The constructor.
 * @param noteListModel The notes list view model to update when the synchronization is finished.
 * @param parent The QObject parent instance.
 */
EvernoteSynchronizer::EvernoteSynchronizer() : QObject()
{
    oauthUrl = QStringLiteral("https://%1/oauth?oauth_consumer_key=%2&oauth_signature=%3"
                              "&oauth_signature_method=PLAINTEXT&oauth_timestamp=%4&oauth_callback=nnoauth")
            .arg(HOST_NAME).arg(CONSUMER_KEY).arg(CONSUMER_SECRET).arg(QDateTime::currentMSecsSinceEpoch() / 1000);
}

/**
 * @return The URL to authentcate via Evernote account.
 */
QString EvernoteSynchronizer::createAuthenticationUrl()
{
    QString reply = requestGet(oauthUrl);
    return QStringLiteral("https://%1//OAuth.action?%2").arg(HOST_NAME)
            .arg(reply.left(reply.indexOf(QStringLiteral("&oauth_token_secret"))));
}

/**
 * @brief Authenticates via Evernote account using the temporary OAuth token.
 * The method saves the retrieved from the request OAuth token and Note Store URl to settings->
 * @param url The url with the temporary OAuth token to authentificate.
 */
void EvernoteSynchronizer::authenticate(QString url)
{
    QString oauthMarker = QStringLiteral("?oauth_token=");
    QString token = url.mid(url.indexOf(oauthMarker) + oauthMarker.length());
    QString reply = requestGet(oauthUrl + QStringLiteral("&oauth_token=%1").arg(token));
    foreach (QString param, reply.split('&')) {
        int pos = param.indexOf('=');
        if (param.left(pos).trimmed().compare("oauth_token") == 0) {
            settings->setValue(Settings::AUTH_TOKEN_KEY, param.mid(pos + 1));
        } else if (param.left(pos).trimmed().compare("edam_noteStoreUrl") == 0) {
            settings->setValue(Settings::NOTE_STORE_URL_KEY, param.mid(pos + 1));
        }
    }
}

/**
 * @brief Starts the synchronization process in the separate thread.
 */
void EvernoteSynchronizer::startSynchronization()
{
    watcher.setFuture(QtConcurrent::run(this, &EvernoteSynchronizer::synchronize));
}

/**
 * @return The database manager.
 */
DatabaseManager *EvernoteSynchronizer::getDatabaseManager() const
{
    return databaseManager;
}

/**
 * @brief Sets the database manager.
 * @param databaseManager The database manager to set.
 */
void EvernoteSynchronizer::setDatabaseManager(DatabaseManager *databaseManager)
{
    this->databaseManager = databaseManager;
    emit databaseManagerChanged();
}

/**
 * @return The file helper.
 */
FileHelper *EvernoteSynchronizer::getFileHelper() const
{
    return fileHelper;
}

/**
 * @brief Sets the given file helper.
 * @param fileHelper The file helper to set.
 */
void EvernoteSynchronizer::setFileHelper(FileHelper *fileHelper)
{
    this->fileHelper = fileHelper;
    emit fileHelperChanged();
}

/**
 * @return The note list model.
 */
NoteListModel *EvernoteSynchronizer::getNoteListModel() const
{
    return noteListModel;
}

/**
 * @brief Sets the note list model.
 * @param noteListModel The note list model to set.
 */
void EvernoteSynchronizer::setNoteListModel(NoteListModel *noteListModel)
{
    this->noteListModel = noteListModel;
    connect(&watcher, SIGNAL(finished()), this, SLOT(updateNoteList()));
    emit noteListModelChanged();
}

/**
 * @return The notification manager.
 */
NotificationManager *EvernoteSynchronizer::getNotificationManager() const
{
    return notificationManager;
}

/**
 * @brief Sets the given notification manager.
 * @param value The notification manager to set.
 */
void EvernoteSynchronizer::setNotificationManager(NotificationManager *notificationManager)
{
    this->notificationManager = notificationManager;
    emit notificationManagerChanged();
}

/**
 * @return The object with the application settings->
 */
Settings *EvernoteSynchronizer::getSettings() const
{
    return settings;
}

/**
 * @brief Sets the object with the application settings->
 * @param settings The object with the application settings to set.
 */
void EvernoteSynchronizer::setSettings(Settings *settings)
{
    this->settings = settings;
}

/**
 * @brief Makes the synchronous GET request by the given URL.
 * @return The reply of request as QString.
 */
QString EvernoteSynchronizer::requestGet(QString url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
    QEventLoop eventLoop;
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    return QUrl::fromPercentEncoding(reply->readAll());
}

/**
 * @brief Retrieves all notes from the Evernote service.
 * @param noteStore The object to exchange information about notes from the service.
 * @return The list of notes.
 */
QList<Note> EvernoteSynchronizer::retrieveNotes(NoteStore *noteStore)
{
    SyncChunk syncChunk = noteStore->getSyncChunk(0, 1000, true);;
    QList<Note> notes = syncChunk.notes.value();
    while (syncChunk.chunkHighUSN.isSet() && syncChunk.chunkHighUSN.value() < syncChunk.updateCount) {
        syncChunk = noteStore->getSyncChunk(syncChunk.chunkHighUSN.value(), 1000, true);
        notes.append(syncChunk.notes.value());
    }
    return notes;
}

/**
 * @brief Performs the synchronization process.
 * The synchronization plan is taken from: https://dev.evernote.com/media/pdf/edam-sync.pdf.
 */
void EvernoteSynchronizer::synchronize()
{
    NoteStore *noteStore = new NoteStore(settings->valueToString(Settings::NOTE_STORE_URL_KEY),
                                         settings->valueToString(Settings::AUTH_TOKEN_KEY));
    noteStore->setNoteStoreUrl(settings->valueToString(Settings::NOTE_STORE_URL_KEY));
    loadNotesFromService(noteStore);
    sendNotesToService(noteStore);
    settings->setValue(Settings::LAST_SYNC_TIME, noteStore->getSyncState().currentTime);
}

/**
 * @brief Loads notes from the Evernote service and saves changes to the local database.
 * @param noteStore The object to exchange information about notes from the service.
 */
void EvernoteSynchronizer::loadNotesFromService(NoteStore *noteStore)
{
    foreach (Note note, retrieveNotes(noteStore)) {
        LocalNote *localNote = databaseManager->retrieveLocalNoteByExternalId(note.guid.value());
        if (localNote == NULL) localNote = new LocalNote();
        if (note.deleted.value() != 0) {
            foreach (QString path, localNote->getPicturePaths()) {
                fileHelper->removeFile(path);
            }
            if (localNote->getId() > 0) {
                databaseManager->deleteNoteById(localNote->getId());
                notificationManager->removeNotification(localNote->getId());
            }
            continue;
        }
        saveServiceNoteToLocalNote(noteStore, note, localNote);
        if (localNote->getId() == 0) {
            databaseManager->createNote(localNote);
        } else if (settings->valueToTimestamp(Settings::LAST_SYNC_TIME) < note.updated.value()) {
            foreach (QString hash, localNote->getHashes()) {
                if (!localNote->getDescription().contains(hash)) {
                    QString picturePath = localNote->getPicturePaths().at(localNote->getHashes().indexOf(hash));
                    localNote->removePicturePath(picturePath);
                    localNote->removeHash(hash);
                    fileHelper->removeFile(picturePath);
                }
            }
            databaseManager->updateNote(localNote);
        }
    }
}

/**
 * @brief Sends the local notes to the Evernote service.
 * @param noteStore The object to exchange information about notes from the service.
 */
void EvernoteSynchronizer::sendNotesToService(NoteStore *noteStore)
{
    foreach (LocalNote *localNote, databaseManager->retrieveAllNotes()) {
        if (localNote->isDeleted()) {
            noteStore->deleteNote(localNote->getExternalId());
            databaseManager->deleteNoteById(localNote->getId());
        } else if (!localNote->isSynchronized()) {
            Note note = createNoteByLocalNote(noteStore, localNote);
            if (localNote->getExternalId().isEmpty()) {
                Note newNote = noteStore->createNote(note);
                localNote->setExternalId(newNote.guid.value());
            } else {
                note.guid = localNote->getExternalId();
                noteStore->updateNote(note);
            }
            localNote->setSynchronized(true);
            databaseManager->updateNote(localNote);
        }
    }
}

/**
 * @brief Creates the Note object for sending to Evernote by the data from the given local note.
 * @param noteStore The object to exchange information about notes from the service.
 * @param localNote The local note.
 * @return The Note object with data for sending to Evernote.
 */
Note EvernoteSynchronizer::createNoteByLocalNote(NoteStore *noteStore, LocalNote *localNote)
{
    Note note;
    note.title = localNote->getTitle();
    if (localNote->getPicturePaths().isEmpty()) {
        note.content = createNoteContent(localNote->getDescription());
    } else {
        QList<Resource> resources = localNote->getExternalId().isEmpty()
                ? QList<Resource>() : noteStore->getNote(localNote->getExternalId(), true, true, false, false).resources.value();
        foreach (Resource resource, resources) {
            if (resource.mime.value().contains("image")) resources.removeOne(resource);
        }
        resources.append(createNoteResources(localNote));
        note.resources = resources;
        note.content = createNoteContent(localNote->getDescription(), resources);
    }
    createLocalTagsOnService(noteStore, localNote);
    NoteAttributes attributes;
    attributes.reminderTime = localNote->getReminderTimestamp();
    note.attributes = attributes;
    note.tagGuids = localNote->getTagExternalIds();
    note.tagNames = localNote->getTagNames();
    return note;
}

/**
 * @brief Creates a string with the note content for sending to the Evernote service.
 * @param localNoteDescription The description of the local note.
 * @param resources The list of resources for attaching to the note
 * @return The note content as a string with XML tags.
 */
QString EvernoteSynchronizer::createNoteContent(QString localNoteDescription, QList<Resource> resources) {
    QString attachedResourcesTags = "";
    foreach (Resource resource, resources) {
        attachedResourcesTags = attachedResourcesTags.append("<en-media hash=\"")
                .append(resource.data.value().bodyHash.value().toHex()).append("\" type=\"")
                .append(resource.mime.value()).append("\"/>");
    }
    return QString(NOTE_DESCRIPTION_TEMPLATE).arg(localNoteDescription.append(attachedResourcesTags));
}

/**
 * @brief Creates tags added locally on the Evernote service and saves
 *        the tag external id to the given LocalNote object.
 * @param noteStore The object to exchange information about notes from the service.
 * @param localNote The local note.
 */
void EvernoteSynchronizer::createLocalTagsOnService(NoteStore *noteStore, LocalNote *localNote)
{
    QList<Tag> serviceTags = noteStore->listTags();
    QHashIterator<QString, QString> iterator(localNote->getTags());
    while (iterator.hasNext()) {
        iterator.next();
        if (iterator.value().length() == 0) {
            Tag tag;
            tag.name = iterator.key();
            foreach (Tag serviceTag, serviceTags) {
                if (serviceTag.name.value().compare(tag.name.value()) == 0) {
                    tag.guid = serviceTag.guid.value();
                }
            }
            if (!tag.guid.isSet()) tag.guid = noteStore->createTag(tag).guid.value();
            localNote->addTag(tag.name.value(), tag.guid.value());
        }
    }
}

/**
 * @brief Creates the list of the Resource objects with the note picture as a body and the "image" as its MIME type.
 * @param localNote The LocalNote object with information about resources (picture paths and hashes) to create.
 * @return The list of the Resource objects.
 */
QList<Resource> EvernoteSynchronizer::createNoteResources(LocalNote *localNote)
{
    QList<Resource> resources;
    foreach (QString picturePath, localNote->getPicturePaths()) {
        Resource resource;
        Data data;
        ResourceAttributes attributes;
        attributes.attachment = true;
        QFile file(picturePath);
        file.open(QIODevice::ReadOnly);
        data.body = file.readAll();
        file.close();
        data.bodyHash = QCryptographicHash::hash(data.body, QCryptographicHash::Md5);
        resource.data = data;
        resource.mime = QMimeDatabase().mimeTypeForFile(picturePath).name();
        resource.attributes = attributes;
        resources.append(resource);
    }
    return resources;
}

/**
 * @brief Loads pictures of the given note and saves it to the local file system.
 * @param noteStore The object to exchange information about notes from the service.
 * @param note The Note object with information about the note resources (pictures, for example).
 * @param noteContent The note content to check that the loaded picture is attached to the note.
 * @param localHashes The local pictures hashes to check already loaded pictures.
 * @return The hash map with the pictures hashes as keys and pictures paths as values.
 */
QHash<QString, QString> EvernoteSynchronizer::loadNotePicturesFromService(NoteStore *noteStore, Note note,
                                                                          const QString &noteContent, QList<QString> localHashes)
{
    QHash<QString, QString> hashesAndPaths;
    if (!note.resources.isSet()) return hashesAndPaths;
    foreach (Resource resource, note.resources.value()) {
        QString mime = resource.mime.value();
        QByteArray hexHash = resource.data.value().bodyHash.value().toHex();
        if (resource.data.isSet() && mime.contains("image") && noteContent.contains(hexHash)
                && !localHashes.contains(hexHash)) {
            QString path = fileHelper->generatePictureFullName(mime.mid(mime.indexOf("/") + 1));
            QFile file(path);
            file.open(QIODevice::WriteOnly);
            file.write(noteStore->getResource(resource.guid.value(), true, false, false, false).data.value().body.value());
            file.close();
            hashesAndPaths[hexHash] = path;
        }
    }
    return hashesAndPaths;
}

/**
 * @brief Loads tags of the given note and saves to the hash map not loaded early tags.
 * @param noteStore The object to exchange information about notes from the service.
 * @param note The Note object with information about the note (tags, for example).
 * @param localTagNames The list of local tag names to check already added tags.
 * @return The hash map with the tag names as keys and tag external ids as values.
 */
QHash<QString, QString> EvernoteSynchronizer::loadNoteTagsFromService(NoteStore *noteStore, Note note,
                                                                      QList<QString> localTagNames)
{
    QHash<QString, QString> namesAndExternalIds;
    if (!note.tagGuids.isSet()) return namesAndExternalIds;
    foreach (Guid tagGuid, note.tagGuids.value()) {
        Tag tag = noteStore->getTag(tagGuid);
        if (!localTagNames.contains(tag.name.value())) {
            namesAndExternalIds[tag.name.value()] = tagGuid;
        }
    }
    return namesAndExternalIds;
}

/**
 * @brief Saves the note infromation from the Evernote service to the given LocalNote object.
 *        The method saves the note title, description, information about attached pictures and tags,
 *        note external id and marks the local note as 'synchronized'.
 * @param noteStore The object to exchange information about notes from the service.
 * @param note The Note object with information about the note on the Evernote service.
 * @param localNote The LocalNote object for saving to the database.
 */
void EvernoteSynchronizer::saveServiceNoteToLocalNote(NoteStore *noteStore, Note note, LocalNote *localNote)
{
    QString content = noteStore->getNoteContent(note.guid.value());
    QHash<QString, QString> hashesAndPaths = loadNotePicturesFromService(noteStore, note, content, localNote->getHashes());
    QHash<QString, QString> tagNamesAndExternalIds = loadNoteTagsFromService(noteStore, note, localNote->getTagNames());
    localNote->setTitle(note.title.value());
    localNote->setDescription(content);
    localNote->addPicturePaths(hashesAndPaths.values());
    localNote->addHashes(hashesAndPaths.keys());
    QHashIterator<QString, QString> iterator(tagNamesAndExternalIds);
    while (iterator.hasNext()) {
        iterator.next();
        localNote->addTag(iterator.key(), iterator.value());
    }
    foreach (QString tagExternalId, localNote->getTagExternalIds()) {
        if (tagExternalId.length() > 0 && !note.tagGuids.value().contains(tagExternalId)) {
            localNote->removeTagByExternalId(tagExternalId);
        }
    }
    localNote->setReminderTimestamp(note.attributes.value().reminderTime.value());
    localNote->setExternalId(note.guid.value());
    localNote->setSynchronized(true);
}

/**
 * @brief Sets up anotification if the reminder timestamp of the given note.
 * @param localNote The local note.
 */
void EvernoteSynchronizer::setupNotification(LocalNote *localNote)
{
    if (localNote->getReminderTimestamp() > 0) {
        QDateTime dateTime;
        dateTime.setTime_t(localNote->getReminderTimestamp() / 1000);
        notificationManager->publishNotification(localNote->getId(), localNote->getTitle(),
                            localNote->getDescription(), dateTime);
    } else {
        notificationManager->removeNotification(localNote->getId());
    }
}

/**
 * @brief Update the note list model and reschedules all notifications.
 */
void EvernoteSynchronizer::updateNoteList()
{
    noteListModel->updateModel();
    foreach (LocalNote *localNote, databaseManager->retrieveAllNotes()) {
        setupNotification(localNote);
    }
}
