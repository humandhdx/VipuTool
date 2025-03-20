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
                pageLoader.sourceComponent= center
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
            property string left_capturePath: ""
            property string right_capturePath: ""
            property bool isCamera: false
            property int captureCount: 0
            property int left_captureCount: 0
            property int right_captureCount: 0
            Component.onCompleted: {
                capturePath=cameraManager.currentDirectory()+"/Global_Calibration/Binocular"
                left_capturePath=cameraManager.currentDirectory()+"/Global_Calibration/Left"
                right_capturePath=cameraManager.currentDirectory()+"/Global_Calibration/Right"
                cameraManager.clearCaptureCount(capturePath)
                cameraManager.clearCaptureCount(left_capturePath)
                cameraManager.clearCaptureCount(right_capturePath)
                cameracalibqwrapper.calibration_resource_load()

            }
            Component.onDestruction: {
                cameraManager.stopCamera()
                if(urtrobot_right.arm_connect){
                    urtrobot_right.robot_drag_activate(false)
                }
                cameracalibqwrapper.calibration_resource_unload()
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
                            Button{
                                width: 150
                                height: 40
                                text: "采集左目图片"
                                enabled: isCamera
                                onClicked: {
                                    if(left_capturePath===""){
                                        console.log("请选择保存路径")
                                        nopath.visible=true
                                        return
                                    }
                                    mask.open()
                                    left_captureCount++
                                    cameraManager.start_camera_capture(left_capturePath,0,left_captureCount)
                                    mask.close()
                                }
                            }
                            Item {
                                width: 180
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
                                    width: 120
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
                                        text: left_captureCount
                                    }
                                    FueButton{
                                        anchors.right: parent.right
                                        anchors.rightMargin: 10
                                        anchors.verticalCenter: parent.verticalCenter
                                        width: 16
                                        height: 16
                                        enabled: left_captureCount>0?true:false
                                        Image {
                                            anchors.fill: parent
                                            source: "qrc:/Image/Clear.svg"
                                            opacity: left_captureCount>0?1.0:0.5
                                        }
                                        onClicked: {
                                            cameraManager.clearCaptureCount(left_capturePath)
                                            left_captureCount=0
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
                                text: "采集双目图片"
                                enabled: isCamera
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
                            Button{
                                width: 150
                                height: 40
                                text: "采集右目图片"
                                enabled: isCamera
                                onClicked: {
                                    if(right_capturePath===""){
                                        console.log("请选择保存路径")
                                        return
                                    }
                                    mask.open()
                                    right_captureCount++
                                    cameraManager.start_camera_capture(right_capturePath,1,right_captureCount)
                                    mask.close()
                                }
                            }
                            Item {
                                width: 180
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
                                    width: 120
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
                                        text: right_captureCount
                                    }
                                    FueButton{
                                        anchors.right: parent.right
                                        anchors.rightMargin: 10
                                        anchors.verticalCenter: parent.verticalCenter
                                        width: 16
                                        height: 16
                                        enabled: right_captureCount>0?true:false
                                        Image {
                                            anchors.fill: parent
                                            source: "qrc:/Image/Clear.svg"
                                            opacity: right_captureCount>0?1.0:0.5
                                        }
                                        onClicked: {
                                            cameraManager.clearCaptureCount(right_capturePath)
                                            right_captureCount=0
                                        }
                                    }
                                }
                            }
                        }
                        Row{
                            spacing: 20
                            Rectangle{
                                width: 290
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
                            }
                            Button{
                                width: 110
                                height: 40
                                text: "查看保存地址"
                                onClicked: {
                                    cameraManager.open_path(cameraManager.currentDirectory()+"/Global_Calibration")
                                }
                            }
                            Row{
                                spacing: 20
                                Rectangle{
                                    width: 200
                                    height: 40
                                    radius: 5
                                    color: "#fff"
                                    Button{
                                        anchors.fill: parent
                                        enabled: false
                                    }
                                    Text {
                                        anchors.centerIn: parent
                                        text: qsTr("左目坐标"+cameracalibqwrapper.joint_pos_index_global_right_LeftEye)
                                    }
                                    Button{
                                        anchors.left: parent.left
                                        width: 50
                                        height: 40
                                        text: "-"
                                        onClicked: {
                                            if(cameracalibqwrapper.joint_pos_index_global_right_LeftEye===0){
                                                return
                                            }
                                            cameracalibqwrapper.joint_pos_index_global_right_LeftEye--
                                        }
                                    }
                                    Button{
                                        anchors.right: parent.right
                                        width: 50
                                        height: 40
                                        text: "+"
                                        onClicked: {
                                            if(cameracalibqwrapper.joint_pos_index_global_right_LeftEye===(cameracalibqwrapper.joint_pos_total_num_global_right_LeftEye-1))
                                            {
                                                return
                                            }
                                            cameracalibqwrapper.joint_pos_index_global_right_LeftEye++
                                        }
                                    }
                                }
                                Button{
                                    width: 130
                                    height: 40
                                    text: "机械臂运动"
                                    onClicked: {
                                        mask.open()
                                        var reslut=urtrobot_right.move_To_Joint_Position_Degree(cameracalibqwrapper.current_joint_pos_global_right_LeftEye)
                                        mask.close()
                                    }
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
                            Row{
                                spacing: 20
                                Rectangle{
                                    width: 200
                                    height: 40
                                    radius: 5
                                    color: "#fff"
                                    Button{
                                        anchors.fill: parent
                                        enabled: false
                                    }
                                    Text {
                                        anchors.centerIn: parent
                                        text: qsTr("右目坐标"+cameracalibqwrapper.joint_pos_index_global_right_RightEye)
                                    }
                                    Button{
                                        anchors.left: parent.left
                                        width: 50
                                        height: 40
                                        text: "-"
                                        onClicked: {
                                            if(cameracalibqwrapper.joint_pos_index_global_right_RightEye===0){
                                                return
                                            }
                                            cameracalibqwrapper.joint_pos_index_global_right_RightEye--
                                        }
                                    }
                                    Button{
                                        anchors.right: parent.right
                                        width: 50
                                        height: 40
                                        text: "+"
                                        onClicked: {
                                            if(cameracalibqwrapper.joint_pos_index_global_right_RightEye===(cameracalibqwrapper.joint_pos_total_num_global_right_RightEye-1))
                                            {
                                                return
                                            }
                                            cameracalibqwrapper.joint_pos_index_global_right_RightEye++
                                        }
                                    }
                                }
                                Button{
                                    width: 130
                                    height: 40
                                    text: "机械臂运动"
                                    onClicked: {
                                        mask.open()
                                        var reslut=urtrobot_right.move_To_Joint_Position_Degree(cameracalibqwrapper.current_joint_pos_global_right_RightEye)
                                        mask.close()
                                    }
                                }
                            }
                        }
                        Row{
                            spacing: 20
                            Item {
                                width: 420
                                height: 40
                                Button{
                                    width: 200
                                    height: 40
                                    text: "开始计算"
                                    onClicked: {
                                        cameraManager.openMalLab()
                                    }
                                }
                            }
                            Row{
                                spacing: 20
                                Rectangle{
                                    width: 200
                                    height: 40
                                    radius: 5
                                    color: "#fff"
                                    Button{
                                        anchors.fill: parent
                                        enabled: false
                                    }
                                    Text {
                                        anchors.centerIn: parent
                                        text: qsTr("双目坐标"+cameracalibqwrapper.joint_pos_index_global_right_DuelEye)
                                    }
                                    Button{
                                        anchors.left: parent.left
                                        width: 50
                                        height: 40
                                        text: "-"
                                        onClicked: {
                                            if(cameracalibqwrapper.joint_pos_index_global_right_DuelEye===0){
                                                return
                                            }
                                            cameracalibqwrapper.joint_pos_index_global_right_DuelEye--
                                        }
                                    }
                                    Button{
                                        anchors.right: parent.right
                                        width: 50
                                        height: 40
                                        text: "+"
                                        onClicked: {
                                            if(cameracalibqwrapper.joint_pos_index_global_right_DuelEye===(cameracalibqwrapper.joint_pos_total_num_global_right_DuelEye-1))
                                            {
                                                return
                                            }
                                            cameracalibqwrapper.joint_pos_index_global_right_DuelEye++
                                        }
                                    }
                                }
                                Button{
                                    width: 130
                                    height: 40
                                    text: "机械臂运动"
                                    onClicked: {
                                        mask.open()
                                        var reslut=urtrobot_right.move_To_Joint_Position_Degree(cameracalibqwrapper.current_joint_pos_global_right_DuelEye)
                                        mask.close()
                                    }
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
    Component{
        id:center
        Item {
            anchors.fill: parent //1260
            property string capturePath: ""
            property string left_capturePath: ""
            property string right_capturePath: ""
            property bool isCamera: false
            property int captureCount: 0
            property int left_captureCount: 0
            property int right_captureCount: 0
            Component.onCompleted: {
                capturePath=cameraManager.currentDirectory()+"/Center_Calibration"
                cameraManager.clearCaptureCount(capturePath)
                cameracalibqwrapper.calibration_resource_load()
            }
            Component.onDestruction: {
                cameraManager.stopCamera()
                if(urtrobot_right.arm_connect){
                    urtrobot_right.robot_drag_activate(false)
                }
                cameracalibqwrapper.calibration_resource_unload()
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
                        id:image
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
                                    isCamera= cameraManager.startCamera(3)
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
                                        console.log("请选择保存路径")
                                        nopath.visible=true
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
                            spacing: 20
                            Rectangle{
                                width: 290
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
                            }
                            Button{
                                width: 110
                                height: 40
                                text: "查看保存地址"
                                onClicked: {
                                    cameraManager.open_path(cameraManager.currentDirectory()+"/Center_Calibration")
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
                            Rectangle{
                                width: 200
                                height: 40
                                radius: 5
                                color: "#fff"
                                Button{
                                    anchors.fill: parent
                                    enabled: false
                                }
                                Text {
                                    anchors.centerIn: parent
                                    text: qsTr(""+cameracalibqwrapper.joint_pos_index_following_right)
                                }
                                Button{
                                    anchors.left: parent.left
                                    width: 50
                                    height: 40
                                    text: "-"
                                    onClicked: {
                                        if(cameracalibqwrapper.joint_pos_index_following_right===0){
                                            return
                                        }
                                        cameracalibqwrapper.joint_pos_index_following_right--
                                    }
                                }
                                Button{
                                    anchors.right: parent.right
                                    width: 50
                                    height: 40
                                    text: "+"
                                    onClicked: {
                                        if(cameracalibqwrapper.joint_pos_index_following_right===(cameracalibqwrapper.joint_pos_total_num_following_right-1))
                                        {
                                            return
                                        }
                                        cameracalibqwrapper.joint_pos_index_following_right++
                                    }
                                }
                            }

                            Button{
                                width: 200
                                height: 40
                                text: "机械臂运动"
                                onClicked: {
                                    mask.open()
                                    var reslut=urtrobot_right.move_To_Joint_Position_Degree(cameracalibqwrapper.current_joint_pos_following_right)
                                    mask.close()
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
            Connections{
                target: image_provider_ml
                function onImgChanged (){
                    image.source=""
                    image.source="image://MlImg/"
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
                    image.source=""
                    cameraManager.stopCamera()
                }
            }
        }

    }

}
