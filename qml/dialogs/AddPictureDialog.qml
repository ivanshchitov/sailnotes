import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {

    property string title
    property string picturePath

    DialogHeader {
        id: header
        title: qsTr("Draw the picture")
    }

    Canvas {
        id: canvas
        anchors {
            left: parent.left
            right: parent.right
            top: header.bottom
            bottom: parent.bottom
            margins: Theme.paddingLarge
        }
        property real lastX
        property real lastY
        property bool filled: false
        onPaint: {
            var ctx = getContext('2d')
            if (!filled) {
                ctx.fillStyle = Qt.rgba(1, 1, 1, 0.5);
                ctx.fillRect(0, 0, width, height);
                filled = true;
            }
            ctx.lineWidth = 5
            ctx.strokeStyle = "black"
            ctx.beginPath()
            ctx.moveTo(lastX, lastY)
            lastX = area.mouseX
            lastY = area.mouseY
            ctx.lineTo(lastX, lastY)
            ctx.stroke()
        }
        MouseArea {
            id: area
            anchors.fill: parent
            onPressed: {
                canvas.lastX = mouseX
                canvas.lastY = mouseY
            }
            onPositionChanged: {
                canvas.requestPaint()
            }
        }
    }
    onDone: {
        picturePath = fileHelper.generatePictureFullName();
        pageStack.backNavigation = true
        canvas.save(picturePath);
        pageStack.forwardNavigation = true
        if (result == DialogResult.Accepted) {
            picturePath = fileHelper.generatePictureFullName();
            canvas.save(picturePath);
        }
    }
    Component.onCompleted: {
        pageStack.backNavigation = false
        pageStack.forwardNavigation = false
    }
}
