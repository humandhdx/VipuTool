import QtQuick
import QtQuick.Controls
Item {
    anchors.fill: parent
    property bool isLeft: false
    Component.onCompleted: {
    }
    Component.onDestruction: {
        //cameraManager.stopCamera()
        cameraManager.stopAllCameras()
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
            text: "左全局相机校准"
            onClicked: {
                isLeft=true
                pageLoader.sourceComponent=left_ameraFoc
            }
        }
        Button{
            width: 200
            height: 40
            text: "右全局相机校准"
            onClicked: {
                isLeft=false
                pageLoader.sourceComponent=right_ameraFoc
            }
        }
    }
    Loader{
        id:pageLoader
        anchors.fill: parent
    }
    Component{
        id:left_ameraFoc
        Item {
            property bool isCamera: false
            Rectangle{
                width: 800
                height:600
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
                    anchors.topMargin: 30
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 390
                    height: 560
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
                                text: "开启相机"
                                enabled: !isCamera
                                onClicked: {
                                    if(isLeft){
                                       isCamera= cameraManager.startCamera(0)
                                        if(isCamera)console.log("相机连接成功")
                                    }
                                    else{
                                       isCamera= cameraManager.startCamera(1)
                                        if(isCamera)console.log("相机连接成功")
                                    }
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
                                text: "开始检测"
                                onClicked: {
                                    cameraManager.startFouc(0)
                                }
                            }
                        }
                    }
                }
            }
            Row{
                anchors.right: parent.right
                anchors.rightMargin: 180
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 120
                spacing: 40
                Text {
                    text: qsTr("最大清晰度："+cameraManager.max_foc.toFixed(2))
                }
                Text {
                    text: qsTr("当前清晰度："+cameraManager.cur_foc.toFixed(2))
                }
                Text {
                    text: qsTr("最小清晰度："+cameraManager.min_foc.toFixed(2))
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
    Component{
        id:right_ameraFoc
        Item {
            property bool isCamera: false
            Rectangle{
                width: 800
                height:600
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
                    anchors.topMargin: 30
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 390
                    height: 560
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
                                text: "开启相机"
                                enabled: !isCamera
                                onClicked: {
                                    if(isLeft){
                                       isCamera= cameraManager.startCamera(0)
                                        if(isCamera)console.log("相机连接成功")
                                    }
                                    else{
                                       isCamera= cameraManager.startCamera(1)
                                        if(isCamera)console.log("相机连接成功")
                                    }
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
                                text: "开始检测"
                                onClicked: {
                                    cameraManager.startFouc(1)
                                }
                            }
                        }
                    }
                }
            }
            Row{
                anchors.right: parent.right
                anchors.rightMargin: 180
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 120
                spacing: 40
                Text {
                    text: qsTr("最大清晰度："+cameraManager.max_foc.toFixed(2))
                }
                Text {
                    text: qsTr("当前清晰度："+cameraManager.cur_foc.toFixed(2))
                }
                Text {
                    text: qsTr("最小清晰度："+cameraManager.min_foc.toFixed(2))
                }
            }
            Connections{
                target: image_provider_gr
                function onImgChanged (){
                    image.source=""
                    image.source="image://GrImg/"
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

