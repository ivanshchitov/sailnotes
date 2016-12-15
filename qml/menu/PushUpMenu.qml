import QtQuick 2.0
import Sailfish.Silica 1.0

PushUpMenu {

    MenuItem {
        text: qsTr("Tags")
        onClicked: pageStack.push(Qt.resolvedUrl("../pages/TagsPage.qml"))
    }

    MenuItem {
        text: qsTr("Settings")
        onClicked: pageStack.push(Qt.resolvedUrl("../pages/SettingsPage.qml"))
    }
}

