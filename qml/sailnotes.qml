import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.configuration 1.0
import harbour.sailnotes.fileHelper 1.0
import harbour.sailnotes.settings 1.0
import harbour.sailnotes.noteListModel 1.0
import harbour.sailnotes.databaseManager 1.0
import harbour.sailnotes.notificationManager 1.0
import harbour.sailnotes.evernoteSynchronizer 1.0

import harbour.sailnotes 1.0
import "pages"

ApplicationWindow
{
    id: appWindow
    initialPage: Component { MainPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: Orientation.All
    _defaultPageOrientations: Orientation.All

    Settings { id: settings }
    FileHelper { id: fileHelper }
    NoteListModel { id: noteListModel }
    DatabaseManager { id: databaseManager }
    NotificationManager { id: notificationManager }
    EvernoteSynchronizer {
        id: evernoteSynchronizer
        noteListModel: noteListModel
        databaseManager: databaseManager
        notificationManager: notificationManager
        settings: settings
        fileHelper: fileHelper
    }
    AudioRecorder { id: audioRecorder }

    ConfigurationValue {
        id: timeFormatConfig
        key: "/sailfish/i18n/lc_timeformat24h"
    }

    function openAddNoteDialog() {
        var localNote = createLocalNote("", "", "", "", "", "", 0);
        var dialog = pageStack.push(Qt.resolvedUrl("dialogs/EditNoteDialog.qml"), {localNote: localNote});
        dialog.accepted.connect(function() {
            var noteId = databaseManager.createNote(dialog.localNote);
            noteListModel.updateModel();
            evernoteSynchronizer.startSynchronization();
            if (dialog.localNote.reminderTimestamp > 0) {
                notificationManager.publishNotification(noteId, dialog.localNote.title, dialog.localNote.description,
                                                        new Date(dialog.localNote.reminderTimestamp));
            }
        });
        dialog.rejected.connect(function() {
            dialog.localNote.getPicturePaths().forEach(function(path) {
                fileHelper.removeFile(path);
            });
        });
    }

    function createLocalNote(title, description, picturePath, hashes, tagNames, tagExternalIds, reminderTimestamp) {
        var localNote = Qt.createQmlObject("import QtQuick 2.0; import harbour.sailnotes.localNote 1.0;
                                            LocalNote {}", parent);
        localNote.title = title;
        localNote.description = description;
        localNote.picturePaths = picturePath;
        localNote.hashes = hashes;
        localNote.setTagsFromStrings(tagNames, tagExternalIds);
        localNote.reminderTimestamp = reminderTimestamp;
        return localNote;
    }
}
