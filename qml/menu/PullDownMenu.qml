import QtQuick 2.0
import Sailfish.Silica 1.0

PullDownMenu {

    MenuItem {
        text: qsTr("Add a new note")
        onClicked: appWindow.openAddNoteDialog();
    }
}
