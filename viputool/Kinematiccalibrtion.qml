import QtQuick
import QtQuick.Controls
Item {
    anchors.fill: parent
    property bool isLeft: false
    Component.onCompleted: {
    }
    Component.onDestruction: {
        if(urtrobot_right.arm_connect){
            urtrobot_right.robot_drag_activate(false)
        }
    }
    Column{
        visible: pageLoader.sourceComponent?false:true
        anchors.centerIn: parent
        spacing: 20
        Button{
            width: 200
            height: 40
            text: "左机械臂标定"
            onClicked: {
                isLeft=true
                pageLoader.sourceComponent=left_arm_cali
            }
        }
        Button{
            width: 200
            height: 40
            text: "右机械臂标定"
            onClicked: {
                isLeft=false
                pageLoader.sourceComponent=left_arm_cali
            }
        }
    }
    Loader{
        id:pageLoader
        anchors.fill: parent
    }
    Component{
        id:left_arm_cali
        Item {
            anchors.fill: parent
            Connections{
                target: logger
                function onSendLogMesseg (msg ,level){
                    //loggerModel.append({"loginfo":msg,"level":level})
                    //loglistview.positionViewAtEnd()
                }
            }
        }
    }
}

