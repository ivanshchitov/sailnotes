import QtQuick 2.0
import Sailfish.Silica 1.0

Page {

    SilicaListView {
        anchors.fill: parent

        VerticalScrollDecorator {}

        header: PageHeader { title: qsTr("Tags") }
        model: ListModel { id: listModel }
        delegate: ListItem {
            width: parent.width
            Column {
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.paddingLarge
                }
                Label {
                    width: parent.width
                    text: tag
                }
                Label {
                    width: parent.width
                    text: numberOfNotes.toString().concat(" ")
                                    .concat(numberOfNotes === 1 ? qsTr("note") : qsTr("notes"))
                    font.pixelSize: Theme.fontSizeExtraSmall
                }
            }
            onClicked: pageStack.push(Qt.resolvedUrl("MainPage.qml"), {tag: tag})
        }
    }
    onStatusChanged: {
        if (status == PageStatus.Activating) {
            listModel.clear();
            databaseManager.retrieveAllTags().forEach(function(tag) {
                listModel.append({tag: tag, numberOfNotes: databaseManager.numberOfNotesWithTag(tag)});
            });
        } else if (status == PageStatus.Active) {
            noteListModel.setNotesTag("");
            noteListModel.updateModel();
        }
    }
}

