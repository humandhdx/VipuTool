import QtQuick
import QtQuick.Controls
import Qt.labs.platform
import VTool 1.0
Item {
    property bool isCamera: false
    property bool isArmConnect: urtrobot_right.arm_connect
    property bool isCalibrtion: false
    property string capturePath: ""
    property double arm1Jpos: 0
    property double arm2Jpos: 0
    property double arm3Jpos: 0
    property double arm4Jpos: 0
    property double arm5Jpos: 0
    property double arm6Jpos: 0
    property double arm7Jpos: 0
    Component.onCompleted: {
        cameraManager.resetCaptureCount()
        capturePath=cameraManager.currentDirectory()
    }
    Component.onDestruction: {
        cameraManager.stopCamera()
    }
    Rectangle{
        width: 800
        height: 600
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
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
        Row{
            spacing: 20
            Button{
                width: 200
                height: 40
                text: "开启相机"
                onClicked: {
                    isCamera= cameraManager.startCamera(2)
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
                        text: isCamera?qsTr("已连接"):qsTr("未连接")
                        color: isCamera?"green":"red"
                    }
                }
            }
        }
        Row{
            spacing: 20
            Button{
                width: 200
                height: 40
                text: "采集图片"
                enabled: isCamera
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
            Item {
                width: 200
                height: 40
                Text {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("采集次数:")
                    font.pixelSize: 12
                    color: isCamera?"#000":"#c9c9c9"
                }
                Rectangle{
                    anchors.right: parent.right
                    width: 140
                    height: 40
                    radius: 5
                    color: "#F5F5F5"
                    border.width: 1
                    border.color: "#c9c9c9"
                }
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
                    font.pixelSize: 12
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
                    folderDialog.selectUrl(capturePath)
                    folderDialog.open()
                    //var result = cameraManager.getCaptureImageSavePath()
                    // if(result!==""){
                    //    capturePath=  result;
                    //    nopath.visible=false
                    // }
                }
            }
        }
        Rectangle{
            width:400
            height: 435
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
                height: 405
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
                    color: "#000000"
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
    Column{
        visible: true
        width: parent.width
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        Text {
            font.pixelSize: 24
            text: qsTr("相机机械臂控制:")
        }
        Rectangle{
            width: parent.width-20
            height: 240
            color: "#D9D9D9"
            Column{
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                spacing: 30
                Row{
                    spacing: 20
                    Button{
                        width: 200
                        height: 40
                        text: "连接机械臂"
                        onClicked: {
                            mask.open()
                            var result= urtrobot_right.robot_connect()
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
                                text: isArmConnect?qsTr("已连接"):qsTr("未连接")
                                color: isArmConnect?"green":"red"
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
                        enabled: isArmConnect
                        onClicked: {
                            //mask.open()
                        }
                    }
                    Button{
                        width: 90
                        height: 40
                        enabled: isArmConnect
                        text: "保持&&记录"
                        onClicked: {
                            //mask.open()
                        }
                    }
                    Item {
                        width: 200
                        height: 40
                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("保存次数:")
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
                        }
                    }
                }
                Row{
                    spacing: 20
                    Button{
                        width: 200
                        height: 40
                        text: "保存数据"
                        onClicked: {
                            //mask.open()
                        }
                    }
                    Button{
                        width: 200
                        height: 40
                        enabled: isCalibrtion
                        text: "手眼计算"
                        onClicked: {
                            //mask.open()
                        }
                    }

                }

            }
            Column{
                anchors.right: parent.right
                anchors.rightMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10
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
                                text: arm1Jpos
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
                                text: arm2Jpos
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
                            text: qsTr("关节3:"+arm3Jpos)
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
                                text: arm3Jpos
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
                                text: arm4Jpos
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
                                text: arm5Jpos
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
                                text: arm6Jpos
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
                            text: arm7Jpos
                        }
                    }
                }
            }
            Row{
                visible: false
                Button{
                    width: 200
                    height: 40
                    text: "开始计算"
                    onClicked: {
                        mask.open()
                        handeyeCulate.startCalibration()
                    }
                }
                Button{
                    width: 200
                    height: 40
                    text: "link"
                    onClicked: {
                        urtrobot_left.robot_connect()
                    }
                }
            }
        }
    }
    VFileDialog {
        id: folderDialog
        mode: VFileDialog.SelectDir
        title:"选择图片保存的文件路径"
        onAccepted: {
            console.log("你选择的文件夹为:"+ folderDialog.currentUrl.toString().replace("file://", ""))
            var localFolder = folderDialog.currentUrl.toString().replace("file://", "");
            capturePath=localFolder
            nopath.visible=false
        }
    }
    Connections{
        target: urtrobot_right
        function onUpdate_Robot_Joint_Pos (current_Jpos){

        }
    }
    Connections{
        target: logger
        function onSendLogMesseg (msg ,level){
            loggerModel.append({"loginfo":msg,"level":level})
            loglistview.positionViewAtEnd()
        }
    }
    Connections{
        target: handeyeCulate
        function onCalculateSucess (){
            mask.close()
        }
    }
    Connections{
        target: image_provider_gl
        function onImgChanged (){
            rightimage.source=""
            rightimage.source="image://GlImg/"
            //rightimage.source="image://GrImg/"+ Math.random()
        }

    }
    Connections{
        target: image_provider_gr
        function onImgChanged (){
            image.source=""
            image.source="image://GrImg/"
        }
    }
    // FolderDialog {
    //     id: folderDialog
    //     title: qsTr("选择图片保存路径")
    //     folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
    //     onAccepted: {
    //         console.log("你选择的文件夹为: " + folderDialog.folder)
    //         var localFolder = folderDialog.folder.toString().replace("file://", "");
    //         capturePath=localFolder
    //         nopath.visible=false
    //     }
    //     onRejected: {
    //         console.log("取消选择")
    //     }
    // }
}
