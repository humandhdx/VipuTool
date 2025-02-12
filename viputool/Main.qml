import QtQuick
import QtQuick.Controls
Window {
    width: 1440
    height: 900
    maximumWidth: 1440
    maximumHeight: 900
    minimumWidth: 1440
    minimumHeight: 900
    visible: true
    title: qsTr("Vipu Tool")
    // VideoOutput 用于显示视频流，source 指定为 cameraManager.camera
    Rectangle{
        anchors.fill: parent
        color: "#FFFFFF"
    }
    Rectangle{
        width: parent.width/8
        height: parent.height
        color: "#D9D9D9"
        Text {
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Apollo Tool")
            color: "#000000"
            font.pixelSize: 24
        }
        ListView{
            id:typelist
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
            width:parent.width-20
            height:900
            model: ListModel{
                ListElement{
                    type:"手眼标定"
                    pageurl:"qrc:/HandEyeCalibration.qml"
                }
                ListElement{
                    type:"全局相机标定"
                    pageurl:"qrc:/GlobalCameraCalibration.qml"
                }
                ListElement{
                    type:"随动相机标定"
                    pageurl:"qrc:/FollowCmaeraCalibration.qml"
                }
            }
            delegate: Item {
                width: parent.width
                height: 45
                Rectangle{
                    anchors.fill: parent
                    radius: 5
                    color:typelist.currentIndex===index ?"#FFFFFF":"#D9D9D9"
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: 18
                        color: "#000000"
                        text: qsTr(""+type)
                    }
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        typelist.currentIndex=index
                        loader.source=pageurl
                    }
                }
            }
            currentIndex: 0
            spacing: 10
        }
    }
    Loader{
        id:loader
        width: parent.width- parent.width/8
        height: parent.height
        anchors.right: parent.right
        source: "qrc:/HandEyeCalibration.qml"
    }
    Popup{
        id:mask
        anchors.centerIn: parent
        width: 300
        height: 180
        modal: true
        closePolicy: Popup.NoAutoClose
        background: Rectangle{
            color: "#00000000"
        }
        contentItem: Rectangle{
            color: "#C9C9C9"
            radius: 20
            anchors.fill: parent
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 10
                font.pixelSize: 24
                text: qsTr("等待完成")
            }
            BusyIndicator {
                anchors.centerIn: parent
                implicitWidth: 96
                implicitHeight: 96
                opacity: 1.0
            }
       }
        Overlay.modal:Rectangle{
            color: Qt.rgba(0, 0, 0, 0.5)
        }

    }

    // Button{
    //     width: 200
    //     height: 50
    //     text:"Switch"
    //     property bool test: true
    //     onClicked: {
    //         if(test){
    //             loader.source="qrc:/HandEyeCalibration.qml"
    //         }
    //         else{
    //           loader.source="qrc:/CameraCalibration.qml"
    //         }
    //         test=!test
    //     }
    // }


}
