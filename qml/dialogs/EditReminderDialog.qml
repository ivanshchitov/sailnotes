import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {

    property date selectedDate
    property int selectedHour
    property int selectedMinute

    property date dateTime
    property bool isNew

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        VerticalScrollDecorator {}

        Column {
            id: column
            width: parent.width

            DialogHeader { title: isNew ? qsTr("Add reminder") : qsTr("Edit reminder") }

            ValueButton {
                width: parent.width
                label: qsTr("Date").concat(": ")
                value: dateTime.getTime() > 0 ? Qt.formatDate(dateTime, "dd MMM yyyy") : qsTr("Select")
                onClicked: {
                    var dialog = pageStack.push("Sailfish.Silica.DatePickerDialog",
                                                { date: dateTime.getTime() === 0 ? 'undefined' : dateTime });
                    dialog.accepted.connect(function() {
                        value = dialog.date.toLocaleDateString([], {year: 'numeric', month: 'long', day: 'numeric'});
                        selectedDate = dialog.date;
                    });
                }
            }

            ValueButton {
                width: parent.width
                label: qsTr("Time").concat(": ")
                value: dateTime.getTime() > 0 ? Qt.formatTime(dateTime,
                                    "hh:mm".concat(timeFormatConfig.value === "24" ? "" : " AP")) : qsTr("Select")

                onClicked: {
                    var dialog = pageStack.push("Sailfish.Silica.TimePickerDialog",
                                                { hour: dateTime.getHours(), minute: dateTime.getMinutes() });
                    dialog.accepted.connect(function() {
                        value = Qt.formatTime(dialog.time, "hh:mm".concat(timeFormatConfig.value === "24" ? "" : " AP"));
                        selectedHour = dialog.hour;
                        selectedMinute = dialog.minute;
                    });
                }
            }

            Button {
                x: Theme.paddingLarge
                width: parent.width - 2 * Theme.paddingLarge
                text: qsTr("Clear reminder")
                visible: dateTime.getTime() > 0
                onClicked: {
                    selectedDate = new Date(0);
                    selectedHour = selectedDate.getHours();
                    selectedMinute = selectedDate.getMinutes();
                    accept();
                }
            }
        }
    }
    onAccepted: {
        dateTime = new Date(selectedDate.getFullYear(), selectedDate.getMonth(),
                            selectedDate.getDate(), selectedHour, selectedMinute);
    }
    Component.onCompleted: {
        if (dateTime.getTime() == 0) {
            dateTime = new Date();
            isNew = true;
        }
        selectedDate = dateTime;
        selectedHour = dateTime.getHours();
        selectedMinute = dateTime.getMinutes();
    }
}
