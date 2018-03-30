import QtQuick 2.0
import Sailfish.Silica 1.0

import "../components"


Dialog {

    property var localNote
    property bool needRemovePicture: false
    property var picturesToRemove: []
    property string placeholder: qsTr("Enter the note description")
    property bool placeholderVisible: localNote.description.length === 0

    canAccept: titleTextField.text.length > 0 || (!placeholderVisible && descriptionTextEdit.text.length > 0)

    SilicaFlickable {

        anchors.fill: parent
        contentHeight: column.height

        VerticalScrollDecorator {}

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingLarge

            DialogHeader {
                id: header
                width: parent.width
                title: localNote.title.length === 0 && localNote.description.length === 0
                       ? qsTr("Add a note") : qsTr("Edit the note")
            }

            ValueButton {
                width: parent.width
                label: qsTr("Reminder").concat(": ")
                value: localNote.reminderTimestamp > 0 ? Qt.formatDateTime(new Date(localNote.reminderTimestamp),
                                    "dd MMM yyyy hh:mm".concat(timeFormatConfig.value === "24" ? "" : " AP")) : qsTr("Select")
                onClicked: {
                    var dialog = pageStack.push("../dialogs/EditReminderDialog.qml",
                                                {dateTime: new Date(localNote.reminderTimestamp)});
                    dialog.accepted.connect(function() {
                        localNote.reminderTimestamp = dialog.dateTime.getTime();
                    });
                }
            }

            TextField {
                id: tagsTextField
                width: parent.width
                text: localNote.tags
                labelVisible: true
                label: qsTr("Tags:")
                placeholderText: qsTr("Enter tags with a comma as a delimiter")
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked:titleTextField.focus = true
            }

            TextField {
                id: titleTextField
                width: parent.width
                text: localNote.title
                labelVisible: true
                label: qsTr("Title:")
                placeholderText: qsTr("Enter the note title")
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: descriptionTextEdit.focus = true
            }

            TextEdit {
                x: Theme.paddingLarge
                id: descriptionTextEdit
                width: parent.width - Theme.paddingLarge
                text: placeholderVisible ? placeholder : localNote.description
                font.pixelSize: Theme.fontSizeMedium
                textFormat: Text.RichText
                color: placeholderVisible ? Theme.secondaryColor : Theme.primaryColor
                wrapMode: TextEdit.Wrap
                inputMethodHints: Qt.ImhNoPredictiveText;
                onCursorPositionChanged: {
                    if (placeholderVisible) cursorPosition = 0;
                }
                onFocusChanged: {
                    if (focus) {
                        color = placeholderVisible ? Theme.secondaryHighlightColor : Theme.highlightColor;
                    } else {
                        color = placeholderVisible ? Theme.secondaryColor : Theme.primaryColor
                    }
                }
                onTextChanged: {
                    var rawText = descriptionTextEdit.getText(0, text.length);
                    if (focus && rawText !== placeholder && placeholderVisible) {
                        text = text.replace(placeholder, "");
                        color = Theme.highlightColor;
                        cursorPosition += 1;
                        placeholderVisible = false;
                    } else if (rawText.length === 0 && !placeholderVisible) {
                        text = placeholder;
                        color = Theme.secondaryHighlightColor;
                        placeholderVisible = true;
                    }
                }
            }

            Row {
                x: Theme.paddingLarge
                width: parent.width - 2 * Theme.paddingLarge
                spacing: Theme.paddingMedium
                Button {

                    width: parent.width / 2 - Theme.paddingMedium
                    text: qsTr("Add a picture")
                    onClicked: {
                        var dialog = pageStack.push(Qt.resolvedUrl("../dialogs/AddPictureDialog.qml"));
                        dialog.accepted.connect(function() {
                            localNote.addPicturePaths(dialog.picturePath);
                            localNote.addHashes(fileHelper.generateFileMD5Hash(dialog.picturePath));
                            gridModel.append({path: dialog.picturePath});
                        });
                    }
                }
                Button {
                    width: parent.width / 2
                    text: qsTr("Add a photo")
                    onClicked: {
                        var dialog = pageStack.push(Qt.resolvedUrl("../pages/CameraPage.qml"));
                        dialog.accepted.connect(function() {
                            localNote.addPicturePaths(dialog.photoPath);
                            localNote.addHashes(fileHelper.generateFileMD5Hash(dialog.picturePath));
                            gridModel.append({path: dialog.photoPath});
                        });
                    }
                }
            }
            AudioPlayer {
                id: audioPlayer
                x: Theme.paddingMedium
                width: parent.width - 2 * Theme.paddingMedium
                audioFilePath: localNote.audioFilePath
            }

            SilicaGridView {
                id: gridView
                x: Theme.paddingLarge
                width: parent.width - 2 * Theme.paddingLarge
                height: cellHeight * (gridModel.count + 1) / 2
                cellWidth: width / 2
                cellHeight: width / 2
                model: ListModel { id: gridModel }

                delegate: Item {
                    x: Theme.paddingLarge
                    y: Theme.paddingLarge
                    width: gridView.cellWidth - Theme.paddingMedium
                    height: gridView.cellHeight - Theme.paddingMedium

                    Column {
                        anchors.fill: parent
                        Image {
                            height: parent.height - deleteButton.height
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            fillMode: Image.PreserveAspectCrop
                            source: path
                        }
                        Button {
                            id: deleteButton
                            width: parent.width
                            text: qsTr("Delete")
                            onClicked: {
                                needRemovePicture = true;
                                picturesToRemove.push(path);
                                localNote.removeHash(localNote.getHashes()[localNote.getPicturePaths().indexOf(path)]);
                                localNote.removePicturePath(path);
                                gridView.model.remove(index);
                            }
                        }
                    }
                }
                Component.onCompleted: {
                    localNote.getPicturePaths().forEach(function (path) {
                        gridModel.append({path: path});
                    });
                }
            }
        }
    }

    onAccepted: {
        localNote.tags = tagsTextField.text;
        localNote.title = titleTextField.text;
        localNote.description = placeholderVisible ? "" : descriptionTextEdit.text
                .substring(descriptionTextEdit.text.indexOf("<body"), descriptionTextEdit.text.indexOf("</body>"))
                .replace(/<body.*?>([\s\S]*)/,"$1");
        localNote.audioFilePath = audioPlayer.audioFileExists ? audioPlayer.audioFilePath : "";
    }
}
