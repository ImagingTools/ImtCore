import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: topPanel
    height: 55
    width: parent.width
    color: "#e5e5e7"
    property string fontName: ""
    property alias model: lvButtons.model
    property alias title: title.text

    gradient: Gradient {
             GradientStop { position: 0.0; color: "#e5e5e7" }
             GradientStop { position: 0.7; color: "#d9d9db" }
             GradientStop { position: 0.98; color: "#d2d2d4" }
             GradientStop { position: 1.0; color: "#a4a5a6" }
         }

    Button {
        id: prevStack
        anchors.verticalCenter: parent.verticalCenter
        x: 10
        width: 30
        height: 30
        iconSource:"../../Icons/Left.svg"
        onClicked: {
            console.log("Left ckicked")
        }
    }

    Button {
        id: nextStack
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: prevStack.right
        anchors.leftMargin: 10
        width: 30
        height: 30
        iconSource:"../../Icons/Right.svg"
    }

    Text {
        id: title
        anchors.left: nextStack.right
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter

        text: qsTr("Products")
        font.family: "Helvetica"
        font.pixelSize: 25
    }

//    TopButton {
//        anchors.centerIn: parent
//        //text: "Test"
//    }

    Item {
        anchors.left: nextStack.right
        anchors.leftMargin: 150
        anchors.right: parent.right
        height: parent.height
        ListView {
            id: lvButtons
//            anchors.horizontalCenter: parent.horizontalCenter
            height: parent.height
            width: contentWidth > parent.width ? parent.width : contentWidth
            model: 4
            clip: true
            orientation: ListView.Horizontal
            delegate: TopButton {
                text: model[CommandEnum.NAME]
                imageSource: model[CommandEnum.ICON]
                fontName: topPanel.fontName
            }
        }
    }
}

