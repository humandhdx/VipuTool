import QtQuick
import QtQuick.Controls
import Qt.labs.platform
import QtQuick.Dialogs
import VTool 1.0
Item {
    property bool isCamera: false
    property bool isArmConnect: urtrobot_right.arm_connect
    property string capturePath: ""
    property string cameraParamePath: ""
    property string armParamePath: ""
    property double arm1Jpos: 0
    property double arm2Jpos: 0
    property double arm3Jpos: 0
    property double arm4Jpos: 0
    property double arm5Jpos: 0
    property double arm6Jpos: 0
    property double arm7Jpos: 0
    property int captureCount: 0
    property int lowestCount: 29
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
                    cameraManager.start_camera_capture(capturePath,1)
                    captureCount++
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
                           cameraManager.clearCaptureCount(capturePath,1)
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
                Text {
                    width: parent.width-15
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 12
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
                width: 80
                height: 40
                text: "修改地址"
                onClicked: {
                    folderDialog.currentFolder =capturePath
                    folderDialog.open()
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
                        enabled: !isArmConnect
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
                            mask.open()
                            var result= urtrobot_right.robot_drag_activate(true)
                            mask.close()
                        }
                    }
                    Button{
                        width: 90
                        height: 40
                        enabled: isArmConnect&&handeyeCulate.arm_pose_count<=captureCount?true:false
                        text: "保持&&记录"
                        onClicked: {
                            mask.open()
                            var result= urtrobot_right.robot_drag_activate(false)
                            var amrposes=[];
                            amrposes.push(arm1Jpos)
                            amrposes.push(arm2Jpos)
                            amrposes.push(arm3Jpos)
                            amrposes.push(arm4Jpos)
                            amrposes.push(arm5Jpos)
                            amrposes.push(arm6Jpos)
                            var result2= handeyeCulate.recordArmPose(amrposes)
                            mask.close()
                        }
                    }
                    Item {
                        width: 200
                        height: 40
                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("保存次数:")
                            color:"#000"
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
                                color: isArmConnect?"#000000":"#c9c9c9"
                                text: handeyeCulate.arm_pose_count
                            }
                            FueButton{
                                anchors.right: parent.right
                                anchors.rightMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                                width: 16
                                height: 16
                                enabled: handeyeCulate.arm_pose_count>0?true:false
                                Image {
                                    anchors.fill: parent
                                    source: "qrc:/Image/Clear.svg"
                                    opacity: handeyeCulate.arm_pose_count>0?1.0:0.5
                                }
                                onClicked: {
                                    handeyeCulate.resetCalibration()
                                }
                            }
                        }
                    }
                }
                Row{
                    spacing: 20
                    Button{
                        width: 200
                        height: 40
                        text: "保存数据"
                        enabled: handeyeCulate.arm_pose_count>0?true:false
                        onClicked: {
                            var result= handeyeCulate.saveArmPose()
                            armParamePath=result
                        }
                    }
                    Button{
                        width: 200
                        height: 40
                        text: "加载全局相机参数"
                        onClicked: {
                            fileDialog.open()
                        }
                    }

                }
            }
            Column{
                anchors.right: parent.right
                anchors.rightMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                spacing: 30
                Row{
                    spacing: 20
                    Item {
                        width: 200
                        height: 40
                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("X:")
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
                                text: arm1Jpos.toFixed(1)
                            }
                        }
                    }
                    Item {
                        width: 200
                        height: 40
                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("Y:")
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
                                text: arm2Jpos.toFixed(1)
                            }
                        }
                    }
                }
                Row{
                    spacing: 20
                    Item {
                        width: 200
                        height: 40
                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("Z:")
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
                                text: arm3Jpos.toFixed(1)
                            }
                        }
                    }
                    Item {
                        width: 200
                        height: 40
                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("RX:")
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
                                text: arm4Jpos.toFixed(1)
                            }
                        }
                    }
                }
                Row{
                    spacing: 20
                    Item {
                        width: 200
                        height: 40
                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("RY:")
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
                                text: arm5Jpos.toFixed(1)
                            }
                        }
                    }
                    Item {
                        width: 200
                        height: 40
                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("RZ:")
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
                                text: arm6Jpos.toFixed(1)
                            }
                        }
                    }
                }
            }
            Button{
                width: 200
                height: 40
                enabled: true
                anchors.centerIn: parent
                text: "开始手眼计算"
                onClicked: {
                    if(captureCount<lowestCount||handeyeCulate.arm_pose_count<lowestCount){
                        console.error("未达到最低采集数据数量")
                        return
                    }
                    if(cameraParamePath==""){
                        console.error("未加载相机参数文件")
                        return
                    }
                    if(armParamePath==""){
                        console.error("未保存机械臂参数")
                        return
                    }
                    if(capturePath==""){
                        console.error("未选择照片保存路径")
                        return
                    }
                    mask.open()
                    var result= handeyeCulate.startCalibration()
                    mask.close()
                }
            }
        }
    }
    Connections{
        target: urtrobot_right
        function onUpdate_Tcp_Cartesian_Pos (current_Jpos){
            arm1Jpos=current_Jpos[0]
            arm2Jpos=current_Jpos[1]
            arm3Jpos=current_Jpos[2]
            arm4Jpos=current_Jpos[3]
            arm5Jpos=current_Jpos[4]
            arm6Jpos=current_Jpos[5]
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
    FolderDialog {
        id: folderDialog
        title: qsTr("选择图片保存路径")
        folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
        onAccepted: {
            var localFolder = folderDialog.folder.toString().replace("file://", "");
            console.log("你选择的文件夹为: " + localFolder)
            capturePath=localFolder
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
    // VFileDialog {
    //     id: folderDialog
    //     mode: VFileDialog.SelectDir
    //     //title:"选择图片保存的文件路径"
    //     onAccepted: {
    //         console.log("你选择的文件夹为:"+ folderDialog.currentUrl.toString().replace("file://", ""))
    //         var localFolder = folderDialog.currentUrl.toString().replace("file://", "");
    //         capturePath=localFolder
    //         nopath.visible=false
    //     }
    // }
}
