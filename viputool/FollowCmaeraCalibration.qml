import QtQuick

Item {
    property bool isCamera: false
    Component.onCompleted: {

    }
    Component.onDestruction: {
        cameraManager.stopCamera()
    }
    Image {
        id:image
        width: 800
        height: 450
        anchors.centerIn: parent
        //anchors.fill: parent
    }
    Connections{
        target: image_provider_ml
        function onImgChanged (){
            image.source=""
            image.source="image://MlImg/"
            //rightimage.source="image://GrImg/"+ Math.random()
        }
    }
}
