import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.6

Item {

    property bool isRecord: false
    property string audioFilePath: ""
    property bool audioFileExists: audioRecorder.isAudioFileExist(audioFilePath)
    property bool allowEditing: true

    height: Theme.itemSizeLarge

    MediaPlayer { id: player }

    Connections {
        target: audioRecorder
        onAudioFileChanged: {
            audioFileExists = audioRecorder.isAudioFileExist(audioFilePath);
        }
    }

    Timer {
        id: timer
        interval: 100
        repeat: true
        onTriggered: {
            if (progressBar.value >= progressBar.maximumValue) {
                timer.stop();
                audioRecorder.stop();
                progressBar.value = 0;
            } else {
                progressBar.value++;
            }
        }
    }

    IconButton {
        id: recordButton
        anchors.left: parent.left
        icon.source: audioRecorder.isRecording
                     ? "../icons/reddot.png" : "image://theme/icon-m-dot"
        enabled: player.playbackState !== MediaPlayer.PlayingState && !audioFileExists
        visible: allowEditing
        onClicked: handleRecordButtonClick()
    }
    IconButton {
        id: playButton
        anchors.left: recordButton.visible ? recordButton.right : parent.left
        icon.source: player.playbackState === MediaPlayer.PlayingState
                     ? "image://theme/icon-m-tabs" : "image://theme/icon-m-media"
        enabled: !audioRecorder.isRecording && audioFileExists
        onClicked: handlePlayButtonClick()
    }
    ProgressBar {
        id: progressBar
        anchors.left: playButton.right
        anchors.right: deleteButton.visible ? deleteButton.left : parent.right
        maximumValue: player.duration > 0 && isRecord == false ? player.duration / 100 : 1200
    }
    IconButton {
        id: deleteButton
        anchors.right: parent.right
        icon.source: "image://theme/icon-m-delete"
        enabled: playButton.enabled
        visible: allowEditing
        onClicked: {
            audioRecorder.removeAudioFile(audioFilePath);
            audioFilePath = "";
        }
    }

    function handleRecordButtonClick() {
        progressBar.value = 0;
        isRecord = true;
        if (audioRecorder.isRecording) {
            audioRecorder.stop();
            timer.stop();
        } else {
            if (audioFilePath.length === 0) {
                audioFilePath = audioRecorder.createAudioFileName();
            }
            audioRecorder.record(audioFilePath);
            timer.start();
        }
    }

    function handlePlayButtonClick() {
        player.source = audioFilePath;
        isRecord = false;
        if (player.playbackState === MediaPlayer.PlayingState) {
            timer.stop();
            progressBar.value = 0;
            player.seek(player.duration);
        } else {
            player.play();
            timer.start();
        }
    }
}
