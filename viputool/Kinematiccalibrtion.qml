import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
Item {
    anchors.fill: parent
    property bool isLeft: false
    Component.onCompleted: {
    }
    Component.onDestruction: {

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
            Component.onCompleted: {
                kinematiccalibqwrapper.calibration_resource_load(true)
            }
            Component.onDestruction: {
                kinematiccalibqwrapper.calibration_resource_unload(true)
                if(urtrobot_left.arm_connect){
                    urtrobot_left.robot_drag_activate(false)
                }
            }
            anchors.fill: parent
            property double arm1Jpos: 0
            property double arm2Jpos: 0
            property double arm3Jpos: 0
            property double arm4Jpos: 0
            property double arm5Jpos: 0
            property double arm6Jpos: 0
            property double arm7Jpos: 0
            property bool isComplete: false
            Rectangle{
                width: 800
                height: 600
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 10
                color: "#D9D9D9"
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 50
                    text: qsTr("机械臂关节状态:")
                    font.pixelSize: 24
                }
                Column{
                    anchors.centerIn: parent
                    // anchors.horizontalCenter: parent.horizontalCenter
                    // anchors.bottom: parent.bottom
                    // anchors.bottomMargin: 20
                    spacing: 20
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
                            width: 370
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("关节2：")
                                font.pixelSize: 24
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
                            width: 370
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("关节3：")
                                font.pixelSize: 24
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
                            width: 370
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("关节4：")
                                font.pixelSize: 24
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
                            width: 370
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("关节5：")
                                font.pixelSize: 24
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
                            width: 370
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("关节6：")
                                font.pixelSize: 24
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
                    Item {
                        width: 370
                        height: 40
                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("关节7：")
                            font.pixelSize: 24
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
                            Button{
                                width: 200
                                height: 40
                                text: "执行下一个点位"
                                enabled: urtrobot_left.arm_connect&&!isComplete
                                onClicked: {
                                    mask.open()
                                    var result= urtrobot_left.move_To_Joint_Position(kinematiccalibqwrapper.calib_target_joint_pose_left)
                                    if(result){
                                        if((kinematiccalibqwrapper.joint_pos_index+1)===kinematiccalibqwrapper.joint_pos_total_num_left){
                                            kinematiccalibqwrapper.joint_pos_index=0
                                            isComplete=true
                                            mask.close()
                                            return
                                        }
                                        kinematiccalibqwrapper.joint_pos_index++
                                    }
                                    else{
                                        console.log("Move_To_Joint_Position_Erro")
                                    }
                                    mask.close()
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "标记当前点位("+kinematiccalibqwrapper.joint_pos_index+")无法观测"
                                enabled: kinematiccalibqwrapper.joint_pos_index!==0&&urtrobot_left.arm_connect&&!isComplete
                                onClicked: {
                                    var result=kinematiccalibqwrapper.add_mask_index_for_position_recorder(kinematiccalibqwrapper.joint_pos_index-1)
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
                                    text: "开启拖拽"
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
                                        var result= urtrobot_left.robot_drag_activate(false)
                                        mask.close()
                                    }
                                }

                            }
                            Button{
                                width: 200
                                height: 40
                                text: "查看机械臂点位数据"
                                onClicked: {
                                    var path=cameraManager.currentDirectory()+"/kinematic_calibration_data/config"
                                    cameraManager.open_path(path)
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "加载跟踪仪数据"
                                enabled: urtrobot_left.arm_connect&&isComplete
                                onClicked: {
                                    fileDialog.open()
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "开始计算"
                                enabled: urtrobot_left.arm_connect
                                onClicked: {
                                    mask.open()
                                    var check=kinematiccalibqwrapper.check_calib_data_ready(true)
                                    if(check){
                                        kinematiccalibqwrapper.add_mask_index_for_position_recorder(38)
                                        kinematiccalibqwrapper.add_mask_index_for_position_recorder(45)
                                        var result=kinematiccalibqwrapper.kinematicCalib_Calculate_Start(true)
                                        if(result){

                                        }
                                    }
                                    mask.close()
                                }
                            }

                        }
                        Row{
                            spacing: 50
                            Item {
                                width: 200
                                height: 40
                                Text {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: qsTr("下个点位:")
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
                                        text: kinematiccalibqwrapper.joint_pos_index
                                    }
                                }
                            }
                            Item {
                                width: 200
                                height: 40
                                Text {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: qsTr("所有点位:")
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
                                        text: kinematiccalibqwrapper.joint_pos_total_num_left
                                    }
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "查看保存的计算结果"
                                enabled: urtrobot_left.arm_connect&&isComplete
                                onClicked: {
                                    var path=cameraManager.currentDirectory()+"/kinematic_calibration_data/output"
                                    cameraManager.open_path(path)
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "写入控制器并重启"
                                enabled: urtrobot_left.arm_connect&&isComplete
                                onClicked: {
                                    var result=urtrobot_left.postLaserCalib_Write_MDH_offset()
                                    if(result){

                                    }
                                }
                            }
                        }
                    }
                }
            }
            FileDialog {
                id: fileDialog
                title: "请选择激光跟踪仪标定文件的保存路径"
                nameFilters: ["文本文件 (*.txt)", "CSV 文件 (*.csv)","所有文件 (*)"]  // 文件类型过滤
                onAccepted: {
                    console.log("请选择激光跟踪仪标定文件的保存路径: " + fileDialog.currentFile.toString().replace("file://", ""))
                    var resource=fileDialog.currentFile.toString().replace("file://", "")
                    kinematiccalibqwrapper.updata_Laser_Observed_Pose(true,resource)
                }
                onRejected: {
                    console.log("激光跟踪仪标定文件选择已取消")
                }
            }
            Connections{
                target: logger
                function onSendLogMesseg (msg ,level){
                    //loggerModel.append({"loginfo":msg,"level":level})
                    //loglistview.positionViewAtEnd()
                }
            }
            Connections{
                target: urtrobot_left
                function onUpdate_Tcp_Cartesian_Pos (current_Jpos){
                    arm1Jpos=current_Jpos[0]
                    arm2Jpos=current_Jpos[1]
                    arm3Jpos=current_Jpos[2]
                    arm4Jpos=current_Jpos[3]
                    arm5Jpos=current_Jpos[4]
                    arm6Jpos=current_Jpos[5]
                }
            }
        }
    }
    Component{
        id:right_arm_cali
        Item {
            Component.onCompleted: {
                kinematiccalibqwrapper.calibration_resource_load(true)
            }
            Component.onDestruction: {
                kinematiccalibqwrapper.calibration_resource_unload(true)
                if(urtrobot_right.arm_connect){
                    urtrobot_right.robot_drag_activate(false)
                }
            }
            anchors.fill: parent
            property double arm1Jpos: 0
            property double arm2Jpos: 0
            property double arm3Jpos: 0
            property double arm4Jpos: 0
            property double arm5Jpos: 0
            property double arm6Jpos: 0
            property double arm7Jpos: 0
            property bool isComplete: false
            Rectangle{
                width: 800
                height: 600
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 10
                color: "#D9D9D9"
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 50
                    text: qsTr("机械臂关节状态:")
                    font.pixelSize: 24
                }
                Column{
                    anchors.centerIn: parent
                    // anchors.horizontalCenter: parent.horizontalCenter
                    // anchors.bottom: parent.bottom
                    // anchors.bottomMargin: 20
                    spacing: 20
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
                            width: 370
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("关节2：")
                                font.pixelSize: 24
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
                            width: 370
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("关节3：")
                                font.pixelSize: 24
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
                            width: 370
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("关节4：")
                                font.pixelSize: 24
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
                            width: 370
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("关节5：")
                                font.pixelSize: 24
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
                            width: 370
                            height: 40
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("关节6：")
                                font.pixelSize: 24
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
                    Item {
                        width: 370
                        height: 40
                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("关节7：")
                            font.pixelSize: 24
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
                                text: "执行下一个点位"
                                enabled: urtrobot_right.arm_connect&&!isComplete
                                onClicked: {
                                    mask.open()
                                    var result= urtrobot_right.move_To_Joint_Position(kinematiccalibqwrapper.calib_target_joint_pose_right)
                                    if(result){
                                        if((kinematiccalibqwrapper.joint_pos_index+1)===kinematiccalibqwrapper.joint_pos_total_num_right){
                                            kinematiccalibqwrapper.joint_pos_index=0
                                            isComplete=true
                                            mask.close()
                                            return
                                        }
                                        kinematiccalibqwrapper.joint_pos_index++
                                    }
                                    else{
                                        console.log("Move_To_Joint_Position_Erro")
                                    }
                                    mask.close()
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text:  "标记当前点位("+kinematiccalibqwrapper.joint_pos_index+")无法观测"
                                enabled: kinematiccalibqwrapper.joint_pos_index!==0&&urtrobot_left.arm_connect&&!isComplete
                                onClicked: {
                                    var result=kinematiccalibqwrapper.add_mask_index_for_position_recorder(kinematiccalibqwrapper.joint_pos_index)
                                    if(result){
                                        kinematiccalibqwrapper.joint_pos_index++
                                    }
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
                                    text: "关闭拖拽"
                                    enabled: urtrobot_right.arm_connect
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
                                text: "查看机械臂点位数据"
                                onClicked: {
                                    var path=cameraManager.currentDirectory()+"/kinematic_calibration_data/config"
                                    cameraManager.open_path(path)
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "加载跟踪仪数据"
                                enabled: urtrobot_right.arm_connect
                                onClicked: {
                                    fileDialog.open()
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "开始计算"
                                enabled: urtrobot_right.arm_connect
                                onClicked: {
                                    mask.open()
                                    var check=kinematiccalibqwrapper.check_calib_data_ready(false)
                                    if(check){
                                        var result=kinematiccalibqwrapper.kinematicCalib_Calculate_Start(false)
                                        if(result){

                                        }
                                    }
                                    mask.close()
                                }
                            }

                        }
                        Row{
                            spacing: 50
                            Item {
                                width: 200
                                height: 40
                                Text {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: qsTr("下个点位:")
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
                                        text: kinematiccalibqwrapper.joint_pos_index
                                    }
                                }
                            }
                            Item {
                                width: 200
                                height: 40
                                Text {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: qsTr("所有点位:")
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
                                        text: kinematiccalibqwrapper.joint_pos_total_num_right
                                    }
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "查看保存的计算结果"
                                enabled: urtrobot_right.arm_connect
                                onClicked: {
                                    var path=cameraManager.currentDirectory()+"/kinematic_calibration_data/output"
                                    cameraManager.open_path(path)
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "写入控制器并重启"
                                enabled: urtrobot_right.arm_connect
                                onClicked: {
                                    mask.open()
                                    var result=urtrobot_right.postLaserCalib_Write_MDH_offset()
                                    if(result){

                                    }
                                    mask.close()
                                }
                            }
                        }
                    }
                }
            }
            FileDialog {
                id: fileDialog
                title: "请选择激光跟踪仪标定文件的保存路径"
                nameFilters: ["文本文件 (*.txt)", "CSV 文件 (*.csv)","所有文件 (*)"]  // 文件类型过滤
                onAccepted: {
                    console.log("请选择激光跟踪仪标定文件的保存路径: " + fileDialog.currentFile.toString().replace("file://", ""))
                    var resource=fileDialog.currentFile.toString().replace("file://", "")
                    kinematiccalibqwrapper.updata_Laser_Observed_Pose(true,resource)
                    var result= kinematiccalibqwrapper.check_calib_data_ready(true)
                    if(result){
                        console.log("加载激光跟踪仪数据成功")
                    }
                    else{
                        console.log("加载激光跟踪仪数据失败")
                    }
                }
                onRejected: {
                    console.log("激光跟踪仪标定文件选择已取消")
                }
            }
            Connections{
                target: logger
                function onSendLogMesseg (msg ,level){
                    //loggerModel.append({"loginfo":msg,"level":level})
                    //loglistview.positionViewAtEnd()
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
        }
    }
}

