import QtQuick 2.0
import Sailfish.Silica 1.0

import "../components"

Page {

    id: page

    property string title
    property string description
    property string picturePaths
    property string tags
    property double reminderTimestamp
    property string audioFilePath

    SilicaFlickable {

        anchors.fill: parent
        contentHeight: column.height

        VerticalScrollDecorator {}

        PageHeader {
            id: header
            width: parent.width
            title: qsTr("Details")
        }

        Column {
            id: column
            width: parent.width
            height: tagsLabel.contentHeight + reminderLabel.contentHeight + titleLabel.contentHeight
                    + descriptionLabel.contentHeight + imageListView.contentHeight
                    + audioPlayer.height + 10 * Theme.paddingLarge
            anchors.top: header.bottom
            spacing: Theme.paddingLarge

            Label {
                id: tagsLabel
                text: qsTr("Tags").concat(": ") + tags
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                visible: tags.length > 0
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Theme.paddingLarge
                }
            }

            Label {
                id: reminderLabel
                text: qsTr("Reminder").concat(": ") + Qt.formatDateTime(new Date(reminderTimestamp),
                                         "dd MMM yyyy hh:mm".concat(timeFormatConfig.value === "24" ? "" : " AP"))
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                visible: reminderTimestamp > 0
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Theme.paddingLarge
                }
            }

            Label {
                id: titleLabel
                text: title
                font.pixelSize: Theme.fontSizeExtraLarge
                wrapMode: Text.Wrap
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Theme.paddingLarge
                }
            }

            Text {
                id: descriptionLabel
                text: description
                font.pixelSize: Theme.fontSizeMedium
                textFormat: Text.RichText
                color: Theme.primaryColor
                wrapMode: Text.Wrap
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Theme.paddingLarge
                }
            }

            AudioPlayer {
                id: audioPlayer
                x: Theme.paddingMedium
                width: parent.width
                audioFilePath: page.audioFilePath
                visible: audioFilePath.length > 0
                allowEditing: false
            }

            SilicaListView {
                id: imageListView
                width: parent.width
                height: listModel.count === 0 ? 0 : contentHeight
                model: ListModel { id: listModel }
                delegate: Image {
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Theme.paddingLarge
                    }
                    fillMode: Image.PreserveAspectFit
                    source: imagePath
                }
                Component.onCompleted: {
                    picturePaths.split(",").forEach(function(path) {
                        listModel.append({imagePath: path});
                    });
                }
            }
        }
    }
}
