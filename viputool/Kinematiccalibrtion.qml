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
                input_cn_popup.open()
            }
        }
        Button{
            width: 200
            height: 40
            text: "右机械臂标定"
            onClicked: {
                isLeft=false
                input_cn_popup.open()
            }
        }
    }
    Loader{
        id:pageLoader
        anchors.fill: parent
    }
    Component{
        id:arm_cali
        Item {
            Component.onCompleted: {
                kinematiccalibqwrapper.calibration_resource_load(isLeft)
                urtrobot_left.robot_set_speed_override(200)
                urtrobot_right.robot_set_speed_override(200)
            }
            Component.onDestruction: {
                kinematiccalibqwrapper.calibration_resource_unload(isLeft)
                if(urtrobot_left.arm_connect){
                    urtrobot_left.robot_drag_activate(false)
                }
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
            property bool isclulate: false
            property var robot: isLeft? urtrobot_left:urtrobot_right
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
                                text: arm7Jpos.toFixed(1)
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
                                enabled: !robot.arm_connect
                                onClicked: {
                                    mask.open()
                                    var result= robot.robot_connect()
                                    if(result) console.log("机械臂成功连接")
                                    else console.error("机械臂连接失败"+robot)
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
                                        text: robot.arm_connect?qsTr("已连接"):qsTr("未连接")
                                        color:robot.arm_connect?"green":"red"
                                    }
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "执行下一个点位"
                                enabled:robot.arm_connect&&!isComplete
                                onClicked: {
                                    mask.open()
                                    var targetPose = isLeft
                                            ? kinematiccalibqwrapper.calib_target_joint_pose_left
                                            : kinematiccalibqwrapper.calib_target_joint_pose_right

                                    var totalNum = isLeft
                                            ? kinematiccalibqwrapper.joint_pos_total_num_left
                                            : kinematiccalibqwrapper.joint_pos_total_num_right

                                    // 执行关节移动
                                    var result = robot.move_To_Joint_Position(targetPose)

                                    // 处理结果
                                    if (result) {
                                        kinematiccalibqwrapper.joint_pos_index += 1
                                        if (kinematiccalibqwrapper.joint_pos_index === totalNum) {
                                            kinematiccalibqwrapper.joint_pos_index = 0
                                            isComplete = true
                                            mask.close()
                                            return
                                        }
                                    } else {
                                        console.log("Move_To_Joint_Position_Error")
                                    }

                                    mask.close()
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "标记当前点位("+kinematiccalibqwrapper.joint_pos_index+")无法观测"
                                enabled: kinematiccalibqwrapper.joint_pos_index!==0&&robot.arm_connect&&!isComplete
                                onClicked: {
                                    kinematiccalibqwrapper.add_mask_index_for_position_recorder(kinematiccalibqwrapper.joint_pos_index-1)
                                    maskmodel.append({maskid:kinematiccalibqwrapper.joint_pos_index-1})
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
                                    enabled:robot.arm_connect
                                    onClicked: {
                                        mask.open()
                                        var result= robot.robot_drag_activate(true)
                                        mask.close()
                                    }
                                }
                                Button{
                                    width: 90
                                    height: 40
                                    text: "关闭拖拽"
                                    enabled: robot.arm_connect
                                    onClicked: {
                                        mask.open()
                                        var result= robot.robot_drag_activate(false)
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
                                enabled: robot.arm_connect
                                onClicked: {
                                    fileDialog.open()
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "开始计算"
                                enabled: robot.arm_connect
                                onClicked: {
                                    isclulate=false
                                    mask.open()
                                    var check=kinematiccalibqwrapper.check_calib_data_ready(isLeft)
                                    if(check){
                                        var result=kinematiccalibqwrapper.kinematicCalib_Calculate_Start(isLeft)
                                        if(result){
                                            isclulate=true
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
                                        text: isLeft?kinematiccalibqwrapper.joint_pos_total_num_left:kinematiccalibqwrapper.joint_pos_total_num_right
                                    }
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "导出计算结果"
                                enabled: robot.arm_connect&&isclulate
                                onClicked: {
                                    mask.open()
                                    var path=cameraManager.currentDirectory()+"/kinematic_calibration_data/output"
                                    kinematiccalibqwrapper.export_Calib_Result(isLeft,path,input_cn_popup.snNub)
                                    cameraManager.open_path(path)
                                    mask.close()
                                }
                            }
                            Button{
                                width: 200
                                height: 40
                                text: "写入控制器并重启"
                                enabled: robot.arm_connect&&isclulate
                                onClicked: {
                                    mask.open()
                                    var result=rotbot.postLaserCalib_Write_MDH_offset()
                                    if(result){

                                    }
                                    mask.close()
                                }
                            }
                        }
                    }
                    ListView{
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        width: 200
                        height: 180
                        orientation: ListView.Vertical
                        boundsBehavior: Flickable.StopAtBounds
                        model: ListModel{
                            id:maskmodel
                        }
                        delegate: Item {
                            width: 180
                            height: 20
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("标记点位id:"+maskid)
                            }
                            Button{
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.right: parent.right
                                text: "删除"
                                onClicked: {
                                    kinematiccalibqwrapper.remove_mask_index_for_position_recorder(maskid)
                                    maskmodel.remove(index)
                                }
                            }
                        }
                        spacing: 5
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
                    kinematiccalibqwrapper.updata_Laser_Observed_Pose(isLeft,resource)
                }
                onRejected: {
                    console.log("激光跟踪仪标定文件选择已取消")
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
                target: urtrobot_right
                function onUpdate_Robot_Joint_Pos (current_Jpos){
                    if(isLeft){
                        return
                    }
                    arm1Jpos=current_Jpos[0]
                    arm2Jpos=current_Jpos[1]
                    arm3Jpos=current_Jpos[2]
                    arm4Jpos=current_Jpos[3]
                    arm5Jpos=current_Jpos[4]
                    arm6Jpos=current_Jpos[5]
                    arm7Jpos=current_Jpos[6]
                }
            }
            Connections{
                target: urtrobot_left
                function onUpdate_Robot_Joint_Pos (current_Jpos){
                    if(!isLeft){
                        return
                    }
                    arm1Jpos=current_Jpos[0]
                    arm2Jpos=current_Jpos[1]
                    arm3Jpos=current_Jpos[2]
                    arm4Jpos=current_Jpos[3]
                    arm5Jpos=current_Jpos[4]
                    arm6Jpos=current_Jpos[5]
                    arm7Jpos=current_Jpos[6]
                }
            }
        }
    }
    Popup{
        id:input_cn_popup
        anchors.centerIn: parent
        width: 400
        height: 240
        modal: true
        closePolicy: Popup.NoAutoClose
        property string snNub: ""
        onClosed: {
            sninput.clear()
        }
        background: Rectangle{
            color: "#00000000"
        }
        contentItem: Rectangle{
            color: "#FFF"
            radius: 20
            anchors.fill: parent
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 10
                font.pixelSize: 24
                text:isLeft? qsTr("输入左机械臂15位SN码"):qsTr("输入右机械臂15位SN码")
            }
            TextField{
                id:sninput
                width: 300
                height: 50
                anchors.centerIn: parent
                placeholderText: "请输入15位字符"
                // 限制最大输入长度为 15
                maximumLength: 15
            }
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                spacing: 20
                Button{
                    width: 120
                    height: 40
                    text: "确定"
                    onClicked: {
                        if(sninput.text.length!=15)
                        {
                            return
                        }
                        input_cn_popup.snNub=isLeft?"CL"+sninput.text:"CR"+sninput.text
                        console.log(input_cn_popup.snNub)
                        pageLoader.sourceComponent=arm_cali
                        input_cn_popup.close()
                    }
                }
                Button{
                    width: 120
                    height: 40
                    text: "取消"
                    onClicked: {
                        input_cn_popup.close()
                    }
                }
            }
        }
        Overlay.modal:Rectangle{
            color: Qt.rgba(0, 0, 0, 0.5)
        }

    }
}

