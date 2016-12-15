import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {

    Label {
        anchors.centerIn: parent
        text: qsTr("SailNotes")
    }

    CoverActionList {

        CoverAction {
            iconSource: "image://theme/icon-cover-new"
            onTriggered: {
                appWindow.openAddNoteDialog();
                appWindow.activate();
            }
        }
    }
}
