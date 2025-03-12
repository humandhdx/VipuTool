import QtQuick
import QtQuick.Controls
Button{
    width: 100
    height: 40
    hoverEnabled: false
    checked: false
    checkable: true
    property color checked_color: "#17CF7D"
    property color normal_color: "#565656"
    property color handle_color: "#fff"
    background : Item{
    }
    contentItem:Rectangle {
        anchors.fill: parent
        radius: height / 2
        color: {
            if(checked){
                return checked_color
            }
            return normal_color
        }
        Rectangle {
            width: parent.height-2
            height: parent.height-2
            radius: width/2
            x:checked ? (parent.width-width)-1 : 1
            anchors.verticalCenter: parent.verticalCenter
            scale: {
                if(pressed){
                    return 0.8
                }
                return 1.0
            }
            color:"#00000000"
            Behavior on scale{
                NumberAnimation{
                    duration: 167
                    easing.type: Easing.OutCubic
                }
            }
            Behavior on x  {
                NumberAnimation {
                    duration: 167
                    easing.type: Easing.OutCubic
                }
            }
            Rectangle{
                anchors.fill: parent
                radius: width/2
                color: handle_color
            }
        }
    }
    onClicked: {

    }
}
