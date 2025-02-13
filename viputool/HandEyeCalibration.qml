import QtQuick
import QtQuick.Controls
Item {
    property bool isCamera: false
    property string capturePath: ""
    Component.onDestruction: {
        cameraManager.stopCamera()
    }
    Rectangle{
        width: 800
        height: 600
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 20
        color: "#D9D9D9"
        Image {
            id:image
            anchors.fill: parent
        }
    }
    Column{
        anchors.left: parent.left
        anchors.leftMargin: 830
        anchors.top: parent.top
        anchors.topMargin: 20
        spacing: 10
        Button{
            width: 200
            height: 40
            text: "开启相机"
            onClicked: {
                isCamera= cameraManager.startCamera(2)
            }
        }
        Button{
            width: 200
            height: 40
            text: "采集图片"
            onClicked: {
                if(capturePath===""){
                    nopath.visible=true
                    return
                }
                enabled=false
                cameraManager.captureImage(capturePath,1)
                enabled=true

            }
        }
        Row{
            spacing: 10
            Rectangle{
                width: 300
                height: 40
                color: "#00000000"
                radius: 5
                border.width: 1
                border.color: "#000000"
                Text {
                    width: parent.width-15
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("保存路径："+capturePath)
                }
                Rectangle{
                    id:nopath
                    visible: false
                    width: 20
                    height: 20
                    radius: width/2
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    color: "red"
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: 16
                        color: "#ffffff"
                        text: qsTr("!")
                    }

                }
            }
            Button{
                width: 80
                height: 40
                text: "修改地址"
                onClicked: {
                    var result = cameraManager.getCaptureImageSavePath()
                    if(result!==""){
                       capturePath=  result;
                       nopath.visible=false
                    }
                }
            }
        }

    }

    // Connections{
    //     target: image_provider_gl
    //     function onImgChanged (){
    //         rightimage.source=""
    //         rightimage.source="image://GlImg/"
    //         //rightimage.source="image://GrImg/"+ Math.random()
    //     }

    // }
    Connections{
        target: image_provider_gr
        function onImgChanged (){
            image.source=""
            image.source="image://GrImg/"
        }
    }

}
