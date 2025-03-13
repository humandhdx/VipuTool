import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
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
            Rectangle{
                width: 1200
                height: 850
                anchors.centerIn: parent
                color: "#D9D9D9"
                RowLayout{
                    width: parent.width/2
                    Layout.preferredHeight:40
                    Layout.minimumHeight:40
                    Button{
                        Layout.fillWidth: true
                        Layout.preferredWidth: 80
                        Layout.maximumWidth: 80
                        Layout.minimumHeight: 40
                        text: "加载机械臂坐标"
                    }
                    Rectangle{
                        Layout.preferredWidth: 300
                        Layout.maximumWidth: 300
                        Layout.minimumHeight: 40
                        color: "#00000000"
                        radius: 5
                        border.width: 1
                        border.color: "#000000"
                        property int dynamicFontSize: {
                             // 假设初始字号为 10
                             var availableWidth = width - 10; // 考虑左右边距
                             var scale = availableWidth / textMetrics.width;
                             // 限制字号范围，防止缩放过小或过大
                             return Math.max(8, Math.min(10, Math.floor(10 * scale)));
                        }
                        TextMetrics {
                             id: textMetrics
                             text: qsTr("保存路径：" )
                             font.pixelSize: 10 // 初始字体大小，计算时会使用这个值
                        }
                        Text {
                            width: parent.width-15
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            font.pixelSize: parent.dynamicFontSize
                            text: qsTr("保存路径")
                            elide: Text.ElideRight
                        }
                    }
                    Button{
                        Layout.fillWidth: true
                        Layout.preferredWidth: 80
                        Layout.maximumWidth: 80
                        Layout.minimumHeight: 40
                        text: "修改地址"
                        onClicked: {
                        }
                    }
                }

            }
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

