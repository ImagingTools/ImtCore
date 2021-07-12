import QtQuick 2.12
import QtGraphicalEffects 1.0

Item {
    id: container
    height: 50
    width: isEmpty ? 20 : 70
    property bool enabled:  true
    property string imageSource: "../Icons/Add.svg"
    property string imageSourceDisabled: "../Icons/Add_On_Disabled.svg"
    //property string imageSourceSelected: "../Icons/Add_On_Selected.svg"
    property string text: "New"
    property string textColor: "#335777"//"#191970"

    property string textColorDisabled: "gray"
    property real fontSize: 12
    property string fontName: ""
    property int radius: 4

    property bool isEmpty: false
    property bool checkable: false
    property bool isChecked: false

    property bool highlighted: false

    property string hint: "button"

    signal clicked()
    signal checked()
    signal hintShow()
    signal hintHide()



    MouseArea{
        id: ma
        anchors.fill: parent
        enabled: container.enabled && container.visible && !container.isEmpty
        hoverEnabled: enabled
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: {
            container.clicked()
        }
        onPressed: {
            container.scale = 0.985
            if(!container.checkable)
                highlighted = true
            else
                container.checked()
        }
        onReleased: {
            container.scale = 1
            if(!container.checkable)
                highlighted = false
        }

        onEntered: container.hintShow()
        onExited: container.hintHide()

    }

    Rectangle{
        id: button
        anchors.top: parent.top
        anchors.topMargin: 6
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 8
        height: parent.height/2
        radius: container.radius
        visible: !container.isEmpty
        Rectangle{
            id: background
            anchors.fill: parent
            color: "#a0a0a0"
            opacity: (checkable && isChecked) ? 0 : highlighted ? 0 : 0.1
        }
        Image {
            id: image
            source: !container.enabled ? imageSourceDisabled : imageSource
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            height: parent.height - 8
            width: height
            sourceSize.width: width
            sourceSize.height: height
        }



    }

    Rectangle{
        id: selection
        anchors.top: button.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: 4
        width: button.width/1.8
        color: "#00FF00"
        visible: checkable ? isChecked : highlighted
    }

    DropShadow{
        id: shadow
        anchors.fill: button
        source: button
        horizontalOffset: 0
        verticalOffset: 1
        color: "#a0a0a0"
        visible: !container.isEmpty
    }

    Text {
        id: description
        anchors.top: selection.bottom
        anchors.topMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter
        height: contentHeight
        text: container.text
        color: container.enabled ? textColor : textColorDisabled
        font.pixelSize: container.fontSize
        font.family: container.fontName
        visible: !container.isEmpty
    }



}
