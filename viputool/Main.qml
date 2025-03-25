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
        Column{
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10
            width: parent.width
            Item {
                width: parent.width
                height: 20
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Apollo Tool")
                    color: "#000000"
                    font.pixelSize: 24
                }
            }
            Item {
                width: parent.width
                height: 20
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("V1.0")
                    color: "#000000"
                    font.pixelSize: 24
                }
            }
        }
        ListView{
            id:typelist
            anchors.top: parent.top
            anchors.topMargin: 100
            anchors.horizontalCenter: parent.horizontalCenter
            width:parent.width-20
            height:900
            interactive: false
            model: ListModel{
                ListElement{
                    type:"相机标定"
                    pageurl:"qrc:/CameraCalibration.qml"
                }
                ListElement{
                    type:"手眼标定"
                    pageurl:"qrc:/HandEyeCalibration.qml"
                }
                ListElement{
                    type:"焦距校准"
                    pageurl:"qrc:/CameraFocus.qml"
                }
                ListElement{
                    type:"机械臂标定"
                    pageurl:"qrc:/Kinematiccalibrtion.qml"
                }
                ListElement{
                    type:"SSH连接"
                    pageurl:"qrc:/SSHLink.qml"
                }
                ListElement{
                    type:"机械臂控制"
                    pageurl:"qrc:/ArmControl.qml"
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
                        mask.open()
                        loader.source=pageurl
                        typelist.currentIndex=index
                        urtrobot_left.spin_until_all_action_finished()
                        urtrobot_right.spin_until_all_action_finished()
                        urtrobot_left.robot_set_speed_override(100)
                        urtrobot_right.robot_set_speed_override(100)
                        mask.close()
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
        source: "qrc:/CameraCalibration.qml"
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
            BusyLoder{
                anchors.centerIn: parent
                implicitWidth: 96
                implicitHeight: 96
            }
        }
        Overlay.modal:Rectangle{
            color: Qt.rgba(0, 0, 0, 0.5)
        }

    }
    /*
    Row{
        spacing: 20
        Item {
            width: 235
            height: 40
            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("关节1:")
                font.pixelSize: 12
            }
            Rectangle{
                anchors.right: parent.right
                width: 175
                height: 40
                radius: 5
                color: "#F5F5F5"
                border.width: 1
                border.color: "#c9c9c9"
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    text: arm1Jpos.toFixed(1)
                }
            }
        }
        Item {
            width: 235
            height: 40
            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("关节2:")
                font.pixelSize: 12
            }
            Rectangle{
                anchors.right: parent.right
                width: 175
                height: 40
                radius: 5
                color: "#F5F5F5"
                border.width: 1
                border.color: "#c9c9c9"
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    text: arm2Jpos.toFixed(1)
                }
            }
        }
    }
    Row{
        spacing: 20
        Item {
            width: 235
            height: 40
            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("关节3:")
                font.pixelSize: 12
            }
            Rectangle{
                anchors.right: parent.right
                width: 175
                height: 40
                radius: 5
                color: "#F5F5F5"
                border.width: 1
                border.color: "#c9c9c9"
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    text: arm3Jpos.toFixed(1)
                }
            }
        }
        Item {
            width: 235
            height: 40
            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("关节4:")
                font.pixelSize: 12
            }
            Rectangle{
                anchors.right: parent.right
                width: 175
                height: 40
                radius: 5
                color: "#F5F5F5"
                border.width: 1
                border.color: "#c9c9c9"
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    text: arm4Jpos.toFixed(1)
                }
            }
        }
    }
    Row{
        spacing: 20
        Item {
            width: 235
            height: 40
            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("关节5:")
                font.pixelSize: 12
            }
            Rectangle{
                anchors.right: parent.right
                width: 175
                height: 40
                radius: 5
                color: "#F5F5F5"
                border.width: 1
                border.color: "#c9c9c9"
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    text: arm5Jpos.toFixed(1)
                }
            }
        }
        Item {
            width: 235
            height: 40
            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("关节6:")
                font.pixelSize: 12
            }
            Rectangle{
                anchors.right: parent.right
                width: 175
                height: 40
                radius: 5
                color: "#F5F5F5"
                border.width: 1
                border.color: "#c9c9c9"
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    text: arm6Jpos.toFixed(1)
                }
            }
        }
    }
    Item {
        width: 235
        height: 40
        Text {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("关节7:")
            font.pixelSize: 12
        }
        Rectangle{
            anchors.right: parent.right
            width: 175
            height: 40
            radius: 5
            color: "#F5F5F5"
            border.width: 1
            border.color: "#c9c9c9"
            Text {
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                text: arm7Jpos.toFixed(1)
            }
        }
    }
    */
}
