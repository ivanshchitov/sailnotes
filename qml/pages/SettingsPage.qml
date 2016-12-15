import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.notifications 1.0
import harbour.sailnotes.settings 1.0

Page {

    property bool authorized: settings.valueToString("auth_token").length !== 0

    // This solution with the JS array as a model for ListView is taken from: https://bugreports.qt.io/browse/QTBUG-11403
    property var listViewModel: [
        {id: "login", title: qsTr("Login via Evernote"), page: "EvernoteAuthPage.qml"},
        {id: "synchronizeManually", title: qsTr("Synchronize data"), page: ""}
    ]

    id: settingsPage

    function setAuthorized(isAuthorized) {
        authorized = isAuthorized;
    }

    Notification {
        id: notification
        previewSummary: qsTr("Synchronization is runned")
    }

    Settings { id: settings }


    SilicaListView {
        id: listView
        anchors.fill: parent
        model: listViewModel
        header: PageHeader { title: qsTr("Settings") }
        delegate: BackgroundItem {
            property var item: model.modelData ? model.modelData : model
            property var itemColor: highlighted ? Theme.highlightColor : Theme.primaryColor;
            width: listView.width
            visible: item.id !== "synchronizeManually" || authorized
            Label {
                x: Theme.horizontalPageMargin
                text: item.id === "login" && authorized ? qsTr("Logout from Evernote") : item.title
                color: itemColor
                anchors.verticalCenter: parent.verticalCenter
            }
            onClicked: {
                if (item.id === "synchronizeManually") {
                    evernoteSynchronizer.startSynchronization();
                    notification.publish();
                } else if (item.id === "login" && authorized) {
                    settings.setValue("auth_token", "");
                    noteListModel.updateModel();
                    setAuthorized(false);
                } else {
                    fileHelper.clearCookies();
                    pageStack.push(Qt.resolvedUrl(item.page), {settingsPage: settingsPage, notification: notification});
                }
            }
        }
    }
}

