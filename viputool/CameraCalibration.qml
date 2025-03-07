import QtQuick
import QtQuick.Controls
import QtMultimedia
import Qt.labs.platform
import QtQuick.Dialogs
Item {
    Column{
        visible: pageLoader.sourceComponent?false:true
        anchors.centerIn: parent
        spacing: 20
        Button{
            width: 200
            height: 40
            text: "全局相机标定"
            onClicked: {
                pageLoader.sourceComponent=global
            }
        }
        Button{
            width: 200
            height: 40
            text: "局部相机标定"
            onClicked: {

            }
        }
        Button{
            width: 200
            height: 40
            text: "随动相机标定"
            onClicked: {

            }
        }
    }
    Loader{
        id:pageLoader
        anchors.fill: parent
        //sourceComponent: global
    }
    Component{
        id:global
        Item {
            anchors.fill: parent //1260
            property string capturePath: ""
            property bool isCamera: false
            property int captureCount: 0
            Component.onCompleted: {
                capturePath=cameraManager.currentDirectory()+"/GlobalImages"
                cameraManager.clearCaptureCount(capturePath)
            }
            Component.onDestruction: {
                cameraManager.stopCamera()
                if(urtrobot_right.arm_connect){
                    urtrobot_right.robot_drag_activate(false)
                }
            }
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 10
                spacing: 5
                Rectangle{
                    width: 620
                    height: 465
                    color: "#D9D9D9"
                    Image {
                        id:leftimage
                        anchors.fill: parent
                    }
                }
                Rectangle{
                    width: 620
                    height: 465
                    color: "#D9D9D9"
                    Image {
                        id:rightimage
                        anchors.fill: parent
                    }
                }
            }
            Column{
                width: parent.width
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                Text {
                    font.pixelSize: 24
                    text: qsTr("相机和机械臂控制:")
                }
                Rectangle{
                    width: parent.width-20
                    height: 380
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
                                text: "开启相机"
                                enabled: !isCamera
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
                                enabled: isCamera&&captureCount<=handeyeCulate.arm_pose_count?true:false
                                onClicked: {
                                    if(capturePath===""){
                                        console.log("请选择保存路径")
                                        nopath.visible=true
                                        return
                                    }
                                    mask.open()
                                    captureCount++
                                    cameraManager.start_camera_capture(capturePath,2,captureCount)
                                    mask.close()
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
                                    color: "#000"
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
                                        color: isCamera?"#000":"#c9c9c9"
                                        text: captureCount
                                    }
                                    FueButton{
                                        anchors.right: parent.right
                                        anchors.rightMargin: 10
                                        anchors.verticalCenter: parent.verticalCenter
                                        width: 16
                                        height: 16
                                        enabled: captureCount>0?true:false
                                        Image {
                                            anchors.fill: parent
                                            source: "qrc:/Image/Clear.svg"
                                            opacity: captureCount>0?1.0:0.5
                                        }
                                        onClicked: {
                                           cameraManager.clearCaptureCount(capturePath)
                                           captureCount=0
                                        }
                                    }
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
                                property int dynamicFontSize: {
                                     // 假设初始字号为 10
                                     var availableWidth = width - 10; // 考虑左右边距
                                     var scale = availableWidth / textMetrics.width;
                                     // 限制字号范围，防止缩放过小或过大
                                     return Math.max(8, Math.min(10, Math.floor(10 * scale)));
                                }
                                TextMetrics {
                                     id: textMetrics
                                     text: qsTr("保存路径：" + capturePath)
                                     font.pixelSize: 10 // 初始字体大小，计算时会使用这个值
                                }
                                Text {
                                    width: parent.width-15
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    font.pixelSize:parent.dynamicFontSize
                                    text: qsTr("保存路径："+capturePath)
                                    elide: Text.ElideRight
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
                                width: 110
                                height: 40
                                text: "修改地址"
                                onClicked: {
                                    folderDialog.currentFolder =capturePath
                                    folderDialog.open()
                                }
                            }
                        }
                        Row{
                            spacing: 20
                            Button{
                                width: 200
                                height: 40
                                text: "连接机械臂"
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
                            Row{
                                spacing: 20
                                Button{
                                    width: 90
                                    height: 40
                                    text: "开启拖拽"
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
                                    enabled: urtrobot_right.arm_connect
                                    text: "关闭拖拽"
                                    onClicked: {
                                        mask.open()
                                        var result= urtrobot_right.robot_drag_activate(false)
                                        mask.close()
                                    }
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "开始计算"
                                onClicked: {
                                    cameraManager.openMalLab()
                                }
                            }
                        }
                    }
                    Rectangle{
                        anchors.right: parent.right
                        anchors.rightMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
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
            }
            FolderDialog {
                id: folderDialog
                title: qsTr("选择图片保存路径")
                folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
                onAccepted: {
                    var localFolder = folderDialog.folder.toString().replace("file://", "");
                    console.log("你选择的文件夹为: " + localFolder)
                    capturePath=localFolder
                    cameraManager.clearCaptureCount(capturePath)
                    captureCount=0
                    nopath.visible=false
                }
                onRejected: {
                    console.log("取消选择")
                }
            }
            FileDialog {
                id: fileDialog
                title: "请选择全局相机参数.yaml文件"
                nameFilters: ["文本文件 (*.yaml)", "所有文件 (*)"]  // 文件类型过滤
                onAccepted: {
                    console.log("选择的全局相机参数文件路径: " + fileDialog.currentFile.toString().replace("file://", ""))
                    cameraParamePath=fileDialog.currentFile.toString().replace("file://", "")
                }
                onRejected: {
                    console.log("全局相机参数文件选择已取消")
                }
            }
            Connections{
                target: image_provider_gl
                function onImgChanged (){
                    leftimage.source=""
                    leftimage.source="image://GlImg/"
                }

            }
            Connections{
                target: image_provider_gr
                function onImgChanged (){
                    rightimage.source=""
                    rightimage.source="image://GrImg/"
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
                target: cameraManager
                function onSignalDeviceErro (){
                    isCamera=false
                    leftimage.source=""
                    rightimage.source=""
                    cameraManager.stopCamera()
                }
            }
        }

    }

}
