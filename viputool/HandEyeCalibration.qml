import QtQuick

Item {
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
        target: image_provider_gl
        function onImgChanged (){
            rightimage.source=""
            rightimage.source="image://GlImg/"
            //rightimage.source="image://GrImg/"+ Math.random()
        }
    }

}
