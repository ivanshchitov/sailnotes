import QtQuick 2.0
import QtMultimedia 5.0
import Sailfish.Silica 1.0

Dialog {

    property string photoPath

    Camera {
        id: camera
        imageCapture {
            onImageSaved: {
                photoPath = path;
                fileHelper.rotatePhoto(path);
                pageStack.backNavigation = true;
                pageStack.forwardNavigation = true;
                accept();
            }
        }
    }
    VideoOutput {
        source: camera
        anchors.fill: parent
        focus : visible
        fillMode: VideoOutput.PreserveAspectFit
        orientation: Orientation.All
        MouseArea {
            anchors.fill: parent
            onClicked: {
                camera.imageCapture.captureToLocation(fileHelper.generatePictureFullName("jpg"));
            }
        }
    }
    Component.onCompleted: {
        pageStack.backNavigation = false;
        pageStack.forwardNavigation = false;
    }
}

