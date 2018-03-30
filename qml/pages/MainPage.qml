import QtQuick 2.0
import Sailfish.Silica 1.0
import "../menu"

Page {

    property string tag

    SilicaListView {

        PullDownMenu { }
        PushUpMenu {
            visible: tag.length === 0
        }

        id: listView
        model: noteListModel
        anchors.fill: parent

        VerticalScrollDecorator {}

        header: PageHeader {
            title: tag.length === 0 ? qsTr("Notes") : tag
        }

        delegate: ListItem {

            id: listItem
            contentHeight: picturePaths.split(",")[0] === "" ?
                               (description.length < 205 ? Theme.itemSizeSmall : Theme.itemSizeLarge) : Theme.itemSizeLarge
            Item {
                id: noteItem
                height: listItem.contentHeight
                width: parent.width

                Label {
                    id: titleLabel
                    width: parent.width
                    anchors {
                        left: parent.left
                        right: image.left
                        top: parent.top
                        leftMargin: Theme.paddingLarge
                    }
                    text: title
                    wrapMode: Text.Wrap
                    maximumLineCount: 1
                }
                Label {
                    id: descriptionLabel
                    anchors {
                        right: image.left
                        left: parent.left
                        top: titleLabel.bottom
                        leftMargin: Theme.paddingLarge
                        rightMargin: Theme.paddingLarge
                    }
                    font.pixelSize: Theme.fontSizeExtraSmall
                    text: description
                    wrapMode: Text.Wrap
                    maximumLineCount: noteItem.height === Theme.itemSizeSmall ? 1 : 2
                }

                Image {
                    id: image
                    width: picturePaths.split(",")[0] === "" ?  0 : parent.width / 4
                    anchors {
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                        leftMargin: Theme.paddingLarge
                    }
                    fillMode: Image.PreserveAspectFit
                    source: picturePaths.split(",")[0]
                }
            }

            onClicked: {
                pageStack.push(Qt.resolvedUrl("NoteDetailsPage.qml"),
                               {title: title, description: description, picturePaths: picturePaths,
                                   tags: tagNames, reminderTimestamp: reminderTimestamp,
                                   audioFilePath: audioFilePath});
            }

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Edit")
                    onClicked: {
                        var localNote = appWindow.createLocalNote(title, description, picturePaths, hashes, tagNames,
                                                                  tagExternalIds, reminderTimestamp);
                        localNote.id = id;
                        localNote.externalId = externalId;
                        localNote.audioFilePath = audioFilePath;
                        var dialog = pageStack.push(Qt.resolvedUrl("../dialogs/EditNoteDialog.qml"), {localNote: localNote});
                        dialog.accepted.connect(function() {
                            if (dialog.needRemovePicture) {
                                dialog.picturesToRemove.forEach(function(path) {
                                    fileHelper.removeFile(path);
                                });
                            }
                            databaseManager.updateNote(dialog.localNote);
                            if (dialog.localNote.reminderTimestamp > 0) {
                                notificationManager.publishNotification(id, dialog.localNote.title, dialog.localNote.description,
                                                                        new Date(dialog.localNote.reminderTimestamp));
                            } else {
                                notificationManager.removeNotification(id);
                            }
                            evernoteSynchronizer.startSynchronization();
                            listView.model.updateModel();
                        });
                    }
                }
                MenuItem {
                    text: qsTr("Delete")
                    onClicked: {
                        appWindow.createLocalNote(title, description, picturePaths, hashes, tagNames,
                                                  tagExternalIds, reminderTimestamp)
                            .getPicturePaths().forEach(function(path) {
                                fileHelper.removeFile(path);
                            });
                        audioRecorder.removeAudioFile(audioFilePath);
                        databaseManager.deleteNoteById(id);
                        notificationManager.removeNotification(id);
                        evernoteSynchronizer.startSynchronization();
                        listView.model.updateModel();
                    }
                }
            }
        }
        Component.onCompleted: {
            noteListModel.setNotesTag(tag);
            noteListModel.updateModel();
            evernoteSynchronizer.startSynchronization();
        }
    }
    onStatusChanged: {
        if (status == PageStatus.Active && tag.length == 0) {
            pageStack.backNavigation = false;
            pageStack.forwardNavigation = false;
        } else if (status == PageStatus.Inactive) {
            pageStack.backNavigation = true;
            pageStack.forwardNavigation = true;
        }
    }
}
