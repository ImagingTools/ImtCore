//import QtQuick 2.0


//Rectangle {
////    anchors.fill: parent
////    property string placeHolderText: "Наименование"

//    width: 200
//    height: 20
//    color: "green"
//}

import QtQuick 2.0
//import QtQuick.Layouts 1.12
//import QtQuick.Controls 2.12
//import QtGraphicalEffects 1.12
//import QtQuick.Controls 2.5
//import QtQuick.Controls.Material 2.12

Item {

    id: container
//    width: parent.width
    height: 50
//    color: "green"//"#fcfcfc"

    signal accepted()

    property string focusColor: "blue"
    property bool enabled:  true

    property alias text: textField.text
    property string placeHolderText: "Наименование"

    property bool canReduce: false
    property bool reduced: false
    property real reduceCoeff: canReduce  ? 2 : 1
    property real animDuration: 200



    onVisibleChanged: {
        if(!visible)
        {
            textField.focus = false

        }

        //textField.text = ""
    }



    TextInput {
        id: textField
//        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 5
//        leftPadding: 0
//        rightPadding: 0
//        topPadding: 0
//        bottomPadding: 0
//        anchors.verticalCenterOffset: height/2 - font.pointSize  - 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        width: parent.width - 20
//        height: parent.height
        color: "#101010"
//        font.pointSize: 12
        font.pixelSize: 15
//        Material.accent: "#101010"
        focus: false
//        placeholderText: ""
        selectByMouse: true
        text: ""
        enabled: container.visible && container.enabled
//        background: Item {
//            width: parent.width
//            height: parent.height
//        }

        onFocusChanged: {

        }

        onTextChanged: {


            if(textField.text == "" && canReduce && reduced)
            {
                container.width *= container.reduceCoeff
                reduced = false
            }

            else if(textField.text !== "" && canReduce && !reduced)
            {
                container.width /= container.reduceCoeff
                reduced = true
            }


        }
        onAccepted: {
            container.accepted()

        }


    }

    Text {
        id: placeHolder
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: margin
        property bool focusOn: textField.focus
        property bool containsText: textField.text !== ""
        property real margin: !focusOn && !containsText ? 12 : !containsText && focusOn
                                                          ? parent.height - height - 3 :  parent.height - height - 3
        Behavior on margin {
            NumberAnimation { duration: animDuration}
        }
        width: contentWidth
        height: contentHeight
        color: focusOn ? focusColor : "gray"
//        font.pointSize: !focusOn && !containsText ? 12 : 9
        font.pixelSize: !focusOn && !containsText ? 15 : 10
        text: container.placeHolderText

    }

    Rectangle{
        id:bottomGray
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 2
        color: "lightgray"
    }

    Rectangle{
        id:bottomBlue
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        height: 3
        width: textField.focus ? parent.width : 0
        visible: textField.focus && !bottomBlue2.visibleState

        Behavior on width {
            NumberAnimation { duration: container.animDuration}
        }
        onWidthChanged:{
            if(width == container.width && canReduce)
                bottomBlue2.visibleState = true
            else if(width == 0 && canReduce)
                bottomBlue2.visibleState = false
        }
        color: focusColor

    }

    Rectangle{
        id: bottomBlue2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        property bool visibleState: false
        height: 3
        width: parent.width
        visible: textField.focus && canReduce &&  bottomBlue2.visibleState

        color: focusColor

    }



}
