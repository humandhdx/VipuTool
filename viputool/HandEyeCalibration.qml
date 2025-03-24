import QtQuick
import QtQuick.Controls
import Qt.labs.platform
import Qt.labs.folderlistmodel
import QtQuick.Dialogs
import VTool 1.0
Item {
    Column{
        visible: pageLoader.sourceComponent?false:true
        anchors.centerIn: parent
        spacing: 20
        Button{
            width: 200
            height: 40
            text: "全局相机手眼标定"
            onClicked: {
                pageLoader.sourceComponent=global
            }
        }
        Button{
            width: 200
            height: 40
            text: "随动相机手眼标定"
            onClicked: {
                pageLoader.sourceComponent=center
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
            property bool isCamera: false
            property string capturePath: ""
            property string savePath: ""
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
            property bool isCaliSucc: false
            Component.onCompleted: {
                capturePath=cameraManager.currentDirectory()+"/GlobalHandEyeImages"
                savePath=cameraManager.currentDirectory()
                cameraManager.clearCaptureCount(capturePath)
                handeyeCulate.resetCalibration()
            }
            Component.onDestruction: {
                cameraManager.stopCamera()
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
                            isCamera= cameraManager.startCamera(0)
                            if(isCamera)console.log("相机连接成功")
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
                        enabled:isCamera&&captureCount<handeyeCulate.arm_pose_count?true:false
                        onClicked: {
                            if(capturePath===""){
                                console.log("请选择保存路径")
                                return
                            }
                            mask.open()
                            captureCount++
                            cameraManager.start_camera_capture(capturePath,0,captureCount)
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
                            Row{
                                anchors.right: parent.right
                                anchors.rightMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 10
                                FueButton{
                                    width: 16
                                    height: 16
                                    enabled: captureCount>0?true:false
                                    Image {
                                        sourceSize.width: 16
                                        sourceSize.height: 16
                                        source: "qrc:/Image/delete.svg"
                                        opacity: captureCount>0?1.0:0.5
                                    }
                                    onClicked: {
                                       var result= cameraManager.deleteFisterCaptureImage(capturePath)
                                       if(result)captureCount--
                                    }
                                }
                                FueButton{
                                    width: 16
                                    height: 16
                                    enabled: captureCount>0?true:false
                                    Image {
                                        anchors.fill: parent
                                        source: "qrc:/Image/Clear.svg"
                                        opacity: captureCount>0?1.0:0.5
                                    }
                                    onClicked: {
                                       var result=cameraManager.clearCaptureCount(capturePath)
                                       handeyeCulate.resetCalibration()
                                       if(result)captureCount=0
                                    }
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
                            font.pixelSize: parent.dynamicFontSize
                            text: qsTr("保存路径"+capturePath)
                            elide: Text.ElideRight
                        }
                    }
                    Button{
                        width: 80
                        height: 40
                        text: "修改地址"
                        onClicked: {
                            folderDialog.folder =capturePath
                            folderDialog.isCamera=true
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
                    text: qsTr("机械臂控制:")
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
                            spacing: 50
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
                            Button{
                                width: 200
                                height: 40
                                enabled: true
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
                                    var result= handeyeCulate.startCalibration(capturePath,armParamePath,cameraParamePath)
                                    mask.close()
                                    isCaliSucc=result
                                }
                            }
                        }
                        Row{
                            spacing: 50
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
                                    enabled: urtrobot_right.arm_connect&&handeyeCulate.arm_pose_count<=captureCount?true:false
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
                                        color: urtrobot_right.arm_connect?"#000000":"#c9c9c9"
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
                                            var result=cameraManager.clearCaptureCount(capturePath)
                                            handeyeCulate.resetCalibration()
                                            captureCount=0
                                        }
                                    }
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                enabled: true
                                text: "保存手眼计算"
                                onClicked: {
                                    if(!isCaliSucc){
                                        console.log("请先完成手眼标定计算")
                                        return
                                    }
                                    var result= handeyeCulate.saveCalibrationToFile(savePath,0)
                                }
                            }
                        }
                        Row{
                            spacing: 50
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
                            Row{
                                spacing: 10
                                Rectangle{
                                    width: 150
                                    height: 40
                                    color: "#00000000"
                                    radius: 5
                                    border.width: 1
                                    border.color: "#000000"
                                    property int dynamicFontSize: {
                                         // 假设初始字号为 10
                                         var availableWidth = width - 10; // 考虑左右边距
                                         var scale = availableWidth / textMetrics2.width;
                                         // 限制字号范围，防止缩放过小或过大
                                         return Math.max(8, Math.min(10, Math.floor(10 * scale)));
                                    }
                                    TextMetrics {
                                         id: textMetrics2
                                         text: qsTr("保存路径：" + capturePath)
                                         font.pixelSize: 10 // 初始字体大小，计算时会使用这个值
                                    }
                                    Text {
                                        width: parent.width-15
                                        anchors.left: parent.left
                                        anchors.leftMargin: 10
                                        anchors.verticalCenter: parent.verticalCenter
                                        font.pixelSize: parent.dynamicFontSize
                                        text: qsTr(savePath)
                                        elide: Text.ElideRight
                                    }
                                    Text {
                                        anchors.bottom: parent.top
                                        anchors.left: parent.left
                                        font.pixelSize: 8
                                        text: qsTr("保存路径")
                                    }
                                }
                                FueButton{
                                    width: 40
                                    height: 40
                                    btncolor:"#00000000"
                                    Image {
                                        sourceSize.width: 40
                                        sourceSize.height: 40
                                        source: "qrc:/Image/filechoice.svg"
                                    }
                                    onClicked: {
                                        folderDialog.folder =savePath
                                        folderDialog.isCamera=false
                                        folderDialog.open()
                                    }
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
               }
            }
            FolderDialog {
                id: folderDialog
                title: qsTr("选择图片保存路径")
                property bool isCamera: false
                onAccepted: {
                    if(isCamera){
                        var localFolder = folderDialog.folder.toString().replace("file://", "");
                        console.log("你选择的图片保存路径为: " + localFolder)
                        capturePath=localFolder
                        cameraManager.clearCaptureCount(capturePath)
                        handeyeCulate.resetCalibration()
                        captureCount=0
                        isCamera=false
                    }
                    else{
                        var Folder = folderDialog.folder.toString().replace("file://", "");
                        console.log("你选择的全局相机手眼参数保存路径为: " + Folder)
                        savePath=Folder
                    }
                }
                onRejected: {
                    console.log("取消选择")
                }
            }
            FileDialog {
                id: fileDialog
                title: "请选择全局相机参数.yaml文件保存路径"
                nameFilters: ["文本文件 (*.yaml)", "所有文件 (*)"]  // 文件类型过滤
                onAccepted: {
                    console.log("请选择全局相机参数.yaml文件保存路径: " + fileDialog.currentFile.toString().replace("file://", ""))
                    cameraParamePath=fileDialog.currentFile.toString().replace("file://", "")
                }
                onRejected: {
                    console.log("全局相机参数文件选择已取消")
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
                target: image_provider_gl
                function onImgChanged (){
                    image.source=""
                    image.source="image://GlImg/"
                }
            }
            Connections{
                target: cameraManager
                function onSignalDeviceErro (){
                    isCamera=false
                    image.source=""
                    cameraManager.stopCamera()
                }
            }
        }
    }
    Component{
        id:center
        Item {
            property bool isCamera: false
            property string capturePath: ""
            property string savePath: ""
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
            property bool isCaliSucc: false
            Component.onCompleted: {
                capturePath=cameraManager.currentDirectory()+"/CenterHandEyeImages"
                savePath=cameraManager.currentDirectory()
                cameraManager.clearCaptureCount(capturePath)
                handeyeCulate.resetCalibration()
            }
            Component.onDestruction: {
                cameraManager.stopCamera()
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
                Image {
                    id:image
                    width: 800
                    height: 450
                    anchors.centerIn: parent
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
                            isCamera= cameraManager.startCamera(3)
                            if(isCamera)console.log("相机连接成功")
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
                        enabled: isCamera&&captureCount<handeyeCulate.arm_pose_count?true:false
                        onClicked: {
                            if(capturePath===""){
                                console.log("请选择保存路径")
                                return
                            }
                            mask.open()
                            captureCount++
                            cameraManager.start_camera_capture(capturePath,3,captureCount)
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
                            Row{
                                anchors.right: parent.right
                                anchors.rightMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 10
                                FueButton{
                                    width: 16
                                    height: 16
                                    enabled: captureCount>0?true:false
                                    Image {
                                        sourceSize.width: 16
                                        sourceSize.height: 16
                                        source: "qrc:/Image/delete.svg"
                                        opacity: captureCount>0?1.0:0.5
                                    }
                                    onClicked: {
                                       var result= cameraManager.deleteFisterCaptureImage(capturePath)
                                       if(result)captureCount--
                                    }
                                }
                                FueButton{
                                    width: 16
                                    height: 16
                                    enabled: captureCount>0?true:false
                                    Image {
                                        anchors.fill: parent
                                        source: "qrc:/Image/Clear.svg"
                                        opacity: captureCount>0?1.0:0.5
                                    }
                                    onClicked: {
                                       var result=cameraManager.clearCaptureCount(capturePath)
                                        handeyeCulate.resetCalibration()
                                       if(result)captureCount=0
                                    }
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
                            font.pixelSize: parent.dynamicFontSize
                            text: qsTr("保存路径"+capturePath)
                            elide: Text.ElideRight
                        }
                    }
                    Button{
                        width: 80
                        height: 40
                        text: "修改地址"
                        onClicked: {
                            folderDialog.folder =capturePath
                            folderDialog.isCamera=true
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
                    text: qsTr("机械臂控制:")
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
                            spacing: 50
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
                            Button{
                                width: 200
                                height: 40
                                enabled: true
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
                                    var result= handeyeCulate.startCalibration(capturePath,armParamePath,cameraParamePath)
                                    mask.close()
                                    isCaliSucc=result
                                }
                            }
                        }
                        Row{
                            spacing: 50
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
                                    enabled: urtrobot_right.arm_connect&&handeyeCulate.arm_pose_count<=captureCount?true:false
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
                                        color: urtrobot_right.arm_connect?"#000000":"#c9c9c9"
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
                                            var result=cameraManager.clearCaptureCount(capturePath)
                                            handeyeCulate.resetCalibration()
                                            captureCount=0
                                        }
                                    }
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                enabled: true
                                text: "保存手眼计算"
                                onClicked: {
                                    if(!isCaliSucc){
                                        console.log("请先完成手眼标定计算")
                                        return
                                    }
                                    var result= handeyeCulate.saveCalibrationToFile(savePath,1)
                                    isCaliSucc=false
                                }
                            }
                        }
                        Row{
                            spacing: 50
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
                                text: "加载随动相机参数"
                                onClicked: {
                                    fileDialog.open()
                                }
                            }
                            Row{
                                spacing: 10
                                Rectangle{
                                    width: 150
                                    height: 40
                                    color: "#00000000"
                                    radius: 5
                                    border.width: 1
                                    border.color: "#000000"
                                    property int dynamicFontSize: {
                                         // 假设初始字号为 10
                                         var availableWidth = width - 10; // 考虑左右边距
                                         var scale = availableWidth / textMetrics2.width;
                                         // 限制字号范围，防止缩放过小或过大
                                         return Math.max(8, Math.min(10, Math.floor(10 * scale)));
                                    }
                                    TextMetrics {
                                         id: textMetrics2
                                         text: qsTr("保存路径：" + capturePath)
                                         font.pixelSize: 10 // 初始字体大小，计算时会使用这个值
                                    }
                                    Text {
                                        width: parent.width-15
                                        anchors.left: parent.left
                                        anchors.leftMargin: 10
                                        anchors.verticalCenter: parent.verticalCenter
                                        font.pixelSize: parent.dynamicFontSize
                                        text: qsTr(savePath)
                                        elide: Text.ElideRight
                                    }
                                    Text {
                                        anchors.bottom: parent.top
                                        anchors.left: parent.left
                                        font.pixelSize: 8
                                        text: qsTr("保存路径")
                                    }
                                }
                                FueButton{
                                    width: 40
                                    height: 40
                                    btncolor:"#00000000"
                                    Image {
                                        sourceSize.width: 40
                                        sourceSize.height: 40
                                        source: "qrc:/Image/filechoice.svg"
                                    }
                                    onClicked: {
                                        folderDialog.folder =savePath
                                        folderDialog.isCamera=false
                                        folderDialog.open()
                                    }
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
               }
            }
            FolderDialog {
                id: folderDialog
                title: qsTr("选择随动相机图片保存路径")
                property bool isCamera: false
                onAccepted: {
                    if(isCamera){
                        var localFolder = folderDialog.folder.toString().replace("file://", "");
                        console.log("你选择的图片保存路径为: " + localFolder)
                        capturePath=localFolder
                        cameraManager.clearCaptureCount(capturePath)
                        handeyeCulate.resetCalibration()
                        captureCount=0
                        isCamera=false
                    }
                    else{
                        var Folder = folderDialog.folder.toString().replace("file://", "");
                        console.log("你选择的随动相机手眼参数保存路径为: " + Folder)
                        savePath=Folder
                    }
                }
                onRejected: {
                    console.log("取消选择")
                }
            }
            FileDialog {
                id: fileDialog
                title: "请选择随动相机参数.yaml文件保存路径"
                nameFilters: ["文本文件 (*.yaml)", "所有文件 (*)"]  // 文件类型过滤
                onAccepted: {
                    console.log("请选择随动相机参数.yaml文件保存路径: " + fileDialog.currentFile.toString().replace("file://", ""))
                    cameraParamePath=fileDialog.currentFile.toString().replace("file://", "")
                }
                onRejected: {
                    console.log("随动相机参数文件选择已取消")
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
                target: image_provider_ml
                function onImgChanged (){
                    image.source=""
                    image.source="image://MlImg/"
                }
            }
            Connections{
                target: cameraManager
                function onSignalDeviceErro (){
                    isCamera=false
                    image.source=""
                    cameraManager.stopCamera()
                }
            }
        }
    }

}
