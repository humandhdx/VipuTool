import QtQuick
import QtQuick.Controls
import QtMultimedia
Item {
    property bool isCamera: false
    Component.onCompleted: {
       cameraManager.startCamera()
    }
    Component.onDestruction: {
        cameraManager.stopCamera()
    }
    Image {
        id:rightimage
        width: 1200
        height: 800
        anchors.centerIn: parent
        //anchors.fill: parent
    }
    // Column{
    //     anchors.top: parent.top
    //     anchors.topMargin: 60
    //     spacing: 10
    //     Button{
    //         width: 200
    //         height: 50
    //         text:"Start"
    //         onClicked: {
    //             isCamera= cameraManager.startCamera(2)
    //             // cameraManager.setVideoOutput(videoOut.videoSink,1)
    //             // var result= cameraManager.startCamera(1)
    //             // if(result) isCamera=true
    //         }
    //     }
    //     Button{
    //         width: 200
    //         height: 50
    //         text:"Capture"
    //         onClicked: {
    //             //cameraManager.captureImage()
    //         }
    //     }
    //     Rectangle{
    //         width: 100
    //         height: 100
    //         radius: width/2
    //         color: isCamera?"green":"red"
    //     }
    // }

    Connections{
        target: image_provider_gr
        function onImgChanged (){
            if(!isCamera){
                return
            }
            rightimage.source=""
            rightimage.source="image://GrImg/"
            //rightimage.source="image://GrImg/"+ Math.random()
        }
    }

}
