import QtQuick 2.0

Item {

    id: container;
    height: 40;

    signal accepted();

    property string focusColor: "blue";
    property bool enabled:  true;

//    property alias text: textField.text;
    property bool isTextChanged: false;
    property string placeHolderText: "Наименование";

    property bool canReduce: false;
    property bool reduced: false;
//    property real reduceCoeff: canReduce  ? 2 : 1;
//    property real animDuration: 200;



    onVisibleChanged: {
        if(!visible)
        {
           // textField.focus = false;
        }
    }

    Rectangle {
        anchors.fill: parent;
        color: "lightgray";
    }

    Timer {
        id: timer;
        interval: 3000;
        onTriggered: {
            console.log("onTriggered",container.isTextChanged);
            if (container.isTextChanged === true){
                container.accepted();
                container.isTextChanged = false;
            }
        }
    }

//    TextInput {
//        id: textField;
//        anchors.left: parent.left;
//        anchors.leftMargin: 5;
//        anchors.bottom: parent.bottom;
//        anchors.bottomMargin: 10;
//        width: parent.width - 20;
//        color: "#101010";
//        font.pixelSize: 15;
//        focus: false;
//        text: "";
//        onFocusChanged: {
////            if (container.isTextChanged === true){
////                console.log("onFocusChanged")
////                container.accepted()
////                container.isTextChanged = false
////            }
//        }

//        onTextChanged: {
//            console.log("onTextChanged");
//            container.isTextChanged = true;
//            timer.restart();
//        }
//        onAccepted: {
//            console.log("onAccepted");
//            container.accepted();
//            container.isTextChanged = false;
//        }

//    }

    Text {
        id: placeHolder;
        anchors.left: parent.left;
        anchors.leftMargin: 5;
        anchors.bottom: parent.bottom;
      //  anchors.bottomMargin: margin;
//        property bool focusOn: textField.focus;
//        property bool containsText: textField.text !== "";
      //  property real margin: !focusOn && !containsText ? 12 : !containsText && focusOn
    //                                                      ? parent.height - height - 3 :  parent.height - height - 3;
//        Behavior on margin {
//            NumberAnimation { duration: animDuration;}
//        }
//        width: contentWidth;
//        height: contentHeight;
//        color: focusOn ? focusColor : "gray";
//        font.pixelSize: !focusOn && !containsText ? 15 : 10;
        //text: container.placeHolderText;

    }

    Rectangle{
        id:bottomGray;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        height: 2;
        color: "lightgray";
    }

    Rectangle{
        id:bottomBlue;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottom: parent.bottom;
        height: 3;
//        width: textField.focus ? parent.width : 0;
//        visible: textField.focus;

        Behavior on width {
            NumberAnimation { duration: container.animDuration;}
        }
        color: focusColor;

    }


}
