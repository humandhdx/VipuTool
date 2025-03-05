import QtQuick
import QtQuick.Controls
Item {
    property bool isLink: false
    Rectangle{
        width: 1240
        height: 880
        anchors.centerIn: parent
        color: "#D9D9D9"
        Row{
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 30
            spacing: 20
            Row{
                Item {
                    width: 35
                    height: 30
                    Text{
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("host:")
                    }
                }
                TextField{
                    id:host
                    width: 200
                    height: 30
                    text: "192.168.1.82"
                }
            }
            Row{
                Item {
                    width: 35
                    height: 30
                    Text{
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("user:")
                    }
                }
                TextField{
                    id:user
                    width: 100
                    height: 30
                    text: "john"
                }
            }
            Row{
                Item {
                    width: 70
                    height: 30
                    Text{
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("password:")
                    }
                }
                TextField{
                    id:password
                    width: 200
                    height: 30
                    text: "foxpg1348"
                }
            }
            Row{
                Item {
                    width: 50
                    height: 30
                    Text{
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("port:")
                    }
                }
                TextField{
                    id:port
                    width: 100
                    height: 30
                    text: "22"
                }
            }
            Button{
                height: 30
                text: "连接"
                onClicked: {
                    var input_host=host.text
                    var input_user=user.text
                    var input_password=password.text
                    var str = port.text; // 获取输入的字符串
                    var portValue = parseInt(str); // 将字符串转换为整数
                    sshManager.sshConnect(input_host,input_user,input_password,str)
                }
            }
            Rectangle{
                width: 30
                height: 30
                radius: 15
                color: isLink?"green":"red"
            }
        }
        Row{
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 80
            spacing: 20
            Item {
                width: 70
                height: 30
                Text{
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("发送命令:")
                }
            }
            TextField{
                id:send
                width: 610
                height: 30
            }
            Button{
                height: 30
                text: "普通发送"
                enabled: isLink
                onClicked: {
                    var str = send.text; // 获取输入的字符串
                    sshManager.sshCommandExecut(str)
                }
            }
            Button{
                height: 30
                text: "管理员发送"
                enabled: isLink
                onClicked: {
                    var str = send.text; // 获取输入的字符串
                    sshManager.sshSudoCommandExecut(str)
                }
            }
        }
        Rectangle{
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.bottom: parent.bottom
            width:400
            height: 350
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
                anchors.topMargin: 30
                anchors.horizontalCenter: parent.horizontalCenter
                width: 390
                height: 320
                clip: true
                orientation: ListView.Vertical
                boundsBehavior: Flickable.StopAtBounds
                snapMode :ListView.NoSnap
                model:ListModel{
                    id:loggerModel
                }
                delegate: TextEdit {
                    function formatLog(input) {
                        // 1. 将 "\n" 和 "\r" 的转义序列转换为实际字符
                        var str = input.replace(/\\n/g, "\n").replace(/\\r/g, "\r");
                        // 2. 将制表符替换为空格（也可以选择直接删除）
                        str = str.replace(/\\t/g, " ");
                        // 3. 删除其它控制字符（ASCII 0x00~0x1F，但保留换行(0x0A)和回车(0x0D)）
                        str = str.replace(/[\x00-\x09\x0B-\x0C\x0E-\x1F]/g, "");
                        return str;
                    }
                    text:formatLog(loginfo) //loginfo.replace(/\\n/g, "\n")
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
    }
    Connections{
        target: sshManager
        function onSshConnected (){
            isLink=true
        }
        function onSshDisConnected (){
            isLink=false
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
