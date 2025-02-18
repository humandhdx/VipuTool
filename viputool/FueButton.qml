import QtQuick
import QtQuick.Controls
Button{
    property string btntext: ""
    property string fontfamily: ""
    property bool fontblod: false
    property color fontcolor: "#00000000"
    property int fontpixelSize:24
    property color btncolor: "#FDFDFD"
    property int btnradius:24
    property string contentDescription: ""
    property real  btnopacity: 1.0
    property real borderwidth: 0
    property color bordercolor: "#00000000"
    property bool btnenable: true
    enabled: btnenable
    hoverEnabled: false
    Text {
        anchors.centerIn: parent
        text: qsTr(""+text)
        font.family:fontfamily
        color: fontcolor
        font.pixelSize:fontpixelSize
        font.bold: fontblod
    }
    background: Rectangle{
        radius: btnradius
        color: btncolor
        border.width: borderwidth
        border.color: bordercolor
    }
    focusPolicy:Qt.TabFocus
    Accessible.role: Accessible.Button
    Accessible.name:btntext
    Accessible.description: contentDescription
    Accessible.onPressAction: control.clicked()
    contentItem: Text {
        text: btntext
        font.bold: fontblod
        font.pixelSize: fontpixelSize
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: fontcolor
    }
    opacity: {
        if(pressed){
            return 0.7
        }
        else{
            return btnopacity
        }
    }
}
