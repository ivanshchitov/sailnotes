import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.0

Item {

    property bool isRecord: false
    height: Theme.itemSizeLarge

    MediaPlayer { id: player }

    Row {
        width: parent.width
        IconButton {
            id: recordButton
            icon.source: "image://theme/icon-m-dot"
        }
        IconButton {
            id: playButton
            icon.source: "image://theme/icon-m-media"
        }
        ProgressBar {
            id: progressBar
            width: parent.width - recordButton.width - playButton.width
            maximumValue: player.duration > 0 && isRecord == false ? player.duration / 100 : 1200
        }
    }
}
