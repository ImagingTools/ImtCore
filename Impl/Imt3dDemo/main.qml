import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import imt3dgui 1.0

Window {
    id: root
    width: 800
    height: 600
    visible: true
    title: "Imt3dDemo — 3D Cube (Scene3DView)"
    color: "#1a1a2e"

    Scene3DView {
        id: view3d
        objectName: "view3d"
        anchors.fill: parent
        cameraDistance: distanceSlider.value
        rotationX: xSlider.value
        rotationY: yRotation
        backgroundColor: "#1a1a2e"

        property real yRotation: 45

        // Auto-rotate when not dragging
        NumberAnimation on yRotation {
            id: autoRotate
            from: view3d.yRotation
            to: view3d.yRotation + 360
            duration: 8000
            loops: Animation.Infinite
            running: !dragArea.pressed
        }
    }

    // Mouse interaction: drag to rotate
    MouseArea {
        id: dragArea
        anchors.fill: parent
        property real lastX: 0
        property real lastY: 0

        onPressed: function(mouse) {
            lastX = mouse.x
            lastY = mouse.y
            autoRotate.stop()
        }

        onPositionChanged: function(mouse) {
            var dx = mouse.x - lastX
            var dy = mouse.y - lastY
            view3d.yRotation += dx * 0.5
            view3d.rotationX = Math.max(-89, Math.min(89, view3d.rotationX - dy * 0.5))
            lastX = mouse.x
            lastY = mouse.y
        }

        onReleased: {
            autoRotate.from = view3d.yRotation
            autoRotate.to = view3d.yRotation + 360
            autoRotate.restart()
        }

        onWheel: function(wheel) {
            var delta = wheel.angleDelta.y / 120.0
            view3d.cameraDistance = Math.max(1.5, Math.min(20.0, view3d.cameraDistance - delta * 0.3))
        }
    }

    // Control panel
    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 16
        width: 220
        height: controlColumn.height + 32
        radius: 12
        color: "#2a2a4a"
        opacity: 0.9

        ColumnLayout {
            id: controlColumn
            anchors.centerIn: parent
            width: parent.width - 32
            spacing: 8

            Text {
                text: "3D Cube Demo (Scene3DView)"
                font.pixelSize: 16
                font.bold: true
                color: "white"
                Layout.alignment: Qt.AlignHCenter
            }

            Text { text: "Rotation X"; color: "#aaa"; font.pixelSize: 11 }
            Slider {
                id: xSlider
                Layout.fillWidth: true
                from: -89; to: 89
                value: view3d.rotationX
                onMoved: view3d.rotationX = value
            }

            Text { text: "Distance"; color: "#aaa"; font.pixelSize: 11 }
            Slider {
                id: distanceSlider
                Layout.fillWidth: true
                from: 1.5; to: 20
                value: 3.0
            }
        }
    }

    // Info label
    Text {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 16
        text: "Drag to rotate · Scroll to zoom · Generic Scene3DView pipeline"
        color: "#666"
        font.pixelSize: 12
    }
}
