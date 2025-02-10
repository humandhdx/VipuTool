import QtQuick
import QtQuick.Controls
import QtMultimedia
Item {
    property bool isCamera: false

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("相机标定")
        font.pixelSize: 64
    }
    VideoOutput {
        id: videoOut
        visible: isCamera
        anchors.fill: parent
        // 可选：调整填充模式

    }
    Column{
        anchors.top: parent.top
        anchors.topMargin: 60
        spacing: 10
        Button{
            width: 200
            height: 50
            text:"Start"
            onClicked: {
                cameraManager.setVideoOutput(videoOut.videoSink,1)
                var result= cameraManager.startCamera(1)
                if(result) isCamera=true
            }
        }
        Button{
            width: 200
            height: 50
            text:"Capture"
            onClicked: {
                cameraManager.captureImage()
            }
        }
        Rectangle{
            width: 100
            height: 100
            radius: width/2
            color: isCamera?"green":"red"
        }
    }



}
