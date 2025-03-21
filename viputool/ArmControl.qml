import QtQuick
import QtQuick.Controls
Item {
    property double left_arm1Jpos: 0
    property double left_arm2Jpos: 0
    property double left_arm3Jpos: 0
    property double left_arm4Jpos: 0
    property double left_arm5Jpos: 0
    property double left_arm6Jpos: 0
    property double left_arm7Jpos: 0
    property double right_arm1Jpos: 0
    property double right_arm2Jpos: 0
    property double right_arm3Jpos: 0
    property double right_arm4Jpos: 0
    property double right_arm5Jpos: 0
    property double right_arm6Jpos: 0
    property double right_arm7Jpos: 0
    Component.onCompleted: {
    }
    Component.onDestruction: {
        if(urtrobot_right.arm_connect){
            urtrobot_right.robot_drag_activate(false)
        }
    }
    Rectangle{
        width: 800
        height: 600
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
        color: "#D9D9D9"
        Column{
            anchors.centerIn: parent
            spacing: 20
            Text {
                font.pixelSize: 24
                text: qsTr("左机械臂关节状态:")
            }
            Row{
                spacing: 20
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节1：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: left_arm1Jpos.toFixed(1)
                        }
                    }
                }
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节2：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: left_arm2Jpos.toFixed(1)
                        }
                    }
                }
            }
            Row{
                spacing: 20
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节3：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: left_arm3Jpos.toFixed(1)
                        }
                    }
                }
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节4：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: left_arm4Jpos.toFixed(1)
                        }
                    }
                }
            }
            Row{
                spacing: 20
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节5：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: left_arm5Jpos.toFixed(1)
                        }
                    }
                }
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节6：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: left_arm6Jpos.toFixed(1)
                        }
                    }
                }
            }
            Item {
                width: 370
                height: 40
                Text {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("关节7：")
                    font.pixelSize: 24
                }
                CustomToggle{
                    anchors.left: parent.left
                    anchors.leftMargin: 100
                    width: 100
                    height: 40
                }
                Rectangle{
                    anchors.right: parent.right
                    width: 140
                    height: 40
                    radius: 5
                    color: "#F5F5F5"
                    border.width: 1
                    border.color: "#c9c9c9"
                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
                        text: left_arm5Jpos.toFixed(1)
                    }
                }
            }
            Text {
                text: qsTr("右机械臂关节状态:")
                font.pixelSize: 24
            }
            Row{
                spacing: 20
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节1：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: right_arm1Jpos.toFixed(1)
                        }
                    }
                }
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节2：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: right_arm2Jpos.toFixed(1)
                        }
                    }
                }
            }
            Row{
                spacing: 20
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节3：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: right_arm3Jpos.toFixed(1)
                        }
                    }
                }
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节4：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: right_arm4Jpos.toFixed(1)
                        }
                    }
                }
            }
            Row{
                spacing: 20
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节5：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: right_arm5Jpos.toFixed(1)
                        }
                    }
                }
                Item {
                    width: 370
                    height: 40
                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("关节6：")
                        font.pixelSize: 24
                    }
                    CustomToggle{
                        anchors.left: parent.left
                        anchors.leftMargin: 100
                        width: 100
                        height: 40
                    }
                    Rectangle{
                        anchors.right: parent.right
                        width: 140
                        height: 40
                        radius: 5
                        color: "#F5F5F5"
                        border.width: 1
                        border.color: "#c9c9c9"
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: right_arm6Jpos.toFixed(1)
                        }
                    }
                }
            }
            Item {
                width: 370
                height: 40
                Text {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("关节7：")
                    font.pixelSize: 24
                }
                CustomToggle{
                    anchors.left: parent.left
                    anchors.leftMargin: 100
                    width: 100
                    height: 40
                }
                Rectangle{
                    anchors.right: parent.right
                    width: 140
                    height: 40
                    radius: 5
                    color: "#F5F5F5"
                    border.width: 1
                    border.color: "#c9c9c9"
                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
                        text: right_arm5Jpos.toFixed(1)
                    }
                }
            }

        }
    }
    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 830
        anchors.top: parent.top
        anchors.topMargin: 10
        width:400
        height: 600
        radius: 5
        color: "#F5F5F5"
        border.width: 1
        border.color: "#000000"
        Text {
            anchors.left: parent.left
            anchors.leftMargin: 5
            font.pixelSize: 24
            text: qsTr("log:")
        }
        Button{
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 5
            width: 50
            height: 24
            text: "清空"
            onClicked: {
                loggerModel.clear()
            }
        }
        ListView{
            id:loglistview
            anchors.top: parent.top
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            width: 390
            height: 550
            clip: true
            orientation: ListView.Vertical
            boundsBehavior: Flickable.StopAtBounds
            snapMode :ListView.NoSnap
            model:ListModel{
                id:loggerModel
            }
            delegate: TextEdit {
                text: loginfo
                font.pixelSize: 12
                width: 395
                wrapMode: Text.Wrap
                color:level===0? "#000000":"red"
                readOnly: true
                // 可选：去除光标和边框，使其看起来更像 Text
                selectByMouse: true
                // 显示选中效果：设置选中区域背景颜色和文字颜色
                selectionColor: "lightblue"      // 选中区域的背景颜色
                selectedTextColor: "black"         // 选中区域中文字的颜色
                cursorVisible: true
                //background: null
            }
            spacing: 2
        }
    }

    Column{
        visible: true
        width: parent.width
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        Text {
            font.pixelSize: 24
            text: qsTr("机械臂控制:")
        }
        Rectangle{
            width: parent.width-20
            height: 240
            color: "#D9D9D9"
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                spacing: 200
                Column{
                    spacing: 30
                    Row{
                        spacing: 50
                        Button{
                            width: 200
                            height: 40
                            text: "连接左机械臂"
                            enabled: !urtrobot_left.arm_connect
                            onClicked: {
                                mask.open()
                                var result= urtrobot_left.robot_connect()
                                if(result) console.log("机械臂成功连接")
                                else console.error("机械臂连接失败")
                                mask.close()
                            }
                        }
                        Item {
                            width: 200
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("连接状态:")
                                font.pixelSize: 12
                            }
                            Rectangle{
                                anchors.right: parent.right
                                width: 140
                                height: 40
                                radius: 5
                                color: "#F5F5F5"
                                border.width: 1
                                border.color: "#c9c9c9"
                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 20
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: urtrobot_left.arm_connect?qsTr("已连接"):qsTr("未连接")
                                    color: urtrobot_left.arm_connect?"green":"red"
                                }
                            }
                        }
                    }
                    Row{
                        spacing: 20
                        Button{
                            width: 90
                            height: 40
                            text: "自由拖拽"
                            enabled: urtrobot_left.arm_connect
                            onClicked: {
                                mask.open()
                                var result= urtrobot_left.robot_drag_activate(true)
                                mask.close()
                            }
                        }
                        Button{
                            width: 90
                            height: 40
                            text: "关闭拖拽"
                            enabled: urtrobot_left.arm_connect
                            onClicked: {
                                mask.open()
                                var result= urtrobot_left.robot_drag_activate(true)
                                mask.close()
                            }
                        }
                    }
                }
                Column{
                    spacing: 30
                    Row{
                        spacing: 50
                        Button{
                            width: 200
                            height: 40
                            text: "连接右机械臂"
                            enabled: !urtrobot_right.arm_connect
                            onClicked: {
                                mask.open()
                                var result= urtrobot_right.robot_connect()
                                if(result) console.log("机械臂成功连接")
                                else console.error("机械臂连接失败")
                                mask.close()
                            }
                        }
                        Item {
                            width: 200
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("连接状态:")
                                font.pixelSize: 12
                            }
                            Rectangle{
                                anchors.right: parent.right
                                width: 140
                                height: 40
                                radius: 5
                                color: "#F5F5F5"
                                border.width: 1
                                border.color: "#c9c9c9"
                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 20
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: urtrobot_right.arm_connect?qsTr("已连接"):qsTr("未连接")
                                    color: urtrobot_right.arm_connect?"green":"red"
                                }
                            }
                        }
                    }
                    Row{
                        spacing: 20
                        Button{
                            width: 90
                            height: 40
                            text: "自由拖拽"
                            enabled: urtrobot_right.arm_connect
                            onClicked: {
                                mask.open()
                                var result= urtrobot_right.robot_drag_activate(true)
                                mask.close()
                            }
                        }
                        Button{
                            width: 90
                            height: 40
                            text: "关闭拖拽"
                            enabled: urtrobot_right.arm_connect
                            onClicked: {
                                mask.open()
                                var result= urtrobot_right.robot_drag_activate(true)
                                mask.close()
                            }
                        }
                    }
                }
            }
        }
    }
    Connections{
        target: urtrobot_left
        function onUpdate_Robot_Joint_Pos (current_Jpos){
            left_arm1Jpos=current_Jpos[0]
            left_arm2Jpos=current_Jpos[1]
            left_arm3Jpos=current_Jpos[2]
            left_arm4Jpos=current_Jpos[3]
            left_arm5Jpos=current_Jpos[4]
            left_arm6Jpos=current_Jpos[5]
            left_arm7Jpos=current_Jpos[6]
        }
    }
    Connections{
        target: urtrobot_right
        function onUpdate_Robot_Joint_Pos (current_Jpos){
            right_arm1Jpos=current_Jpos[0]
            right_arm2Jpos=current_Jpos[1]
            right_arm3Jpos=current_Jpos[2]
            right_arm4Jpos=current_Jpos[3]
            right_arm5Jpos=current_Jpos[4]
            right_arm6Jpos=current_Jpos[5]
            right_arm7Jpos=current_Jpos[6]
        }
    }
    Connections{
        target: logger
        function onSendLogMesseg (msg ,level){
            loggerModel.append({"loginfo":msg,"level":level})
            loglistview.positionViewAtEnd()
        }
    }

}
