import QtQuick
import Qt5Compat.GraphicalEffects

Image {
    id:img
    property alias svgColor:overlay.color
    MouseArea{
        id:msa
        anchors.fill:parent
        hoverEnabled:true
        onEntered:{
            scaleDown.stop()
            scaleUp.restart()
        }
        onExited:{
            scaleUp.stop()
            scaleDown.restart()
        }


    }
    ColorOverlay{
        id:overlay
        anchors.fill:img
        source:img
    }

    scale:0.8
    NumberAnimation on scale{ id:scaleDown; to: 0.8; duration: 200;running:false}
    NumberAnimation on scale{ id:scaleUp; to: 1.; duration: 200;running:false}
}
