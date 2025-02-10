import QtQuick
import QtQuick.Controls
Window {
    width: 1920
    height: 1080
    visible: true
    title: qsTr("Hello World")
    // VideoOutput 用于显示视频流，source 指定为 cameraManager.camera

    Loader{
        id:loader
        anchors.fill: parent
        source: "qrc:/CameraCalibration.qml"

    }

    Button{
        width: 200
        height: 50
        text:"Switch"
        property bool test: true
        onClicked: {
            if(test){
                loader.source="qrc:/HandEyeCalibration.qml"
            }
            else{
              loader.source="qrc:/CameraCalibration.qml"
            }
            test=!test
        }
    }


}
